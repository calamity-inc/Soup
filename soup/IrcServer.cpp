#include "IrcServer.hpp"
#if !SOUP_WASM

#include "Socket.hpp"
#include "string.hpp"

NAMESPACE_SOUP
{
	static void ircChannelMsg(IrcServer* serv, const std::string& channel_name, const std::string& msg, Socket* exclude = nullptr)
	{
		for (const auto& member : serv->getChannelMembers(channel_name))
		{
			if (member.socket != exclude)
			{
				member.socket->send(msg);
			}
		}
	}

	static void ircHandlePart(IrcServer* serv, IrcClientData& cd, const std::string& channel_name)
	{
		if (auto e = serv->channels.find(channel_name); e != serv->channels.end())
		{
			auto members = serv->getChannelMembers(channel_name);

			std::string msg(1, ':');
			msg.append(cd.nick);
			msg.append(" PART :");
			msg.append(channel_name);
			msg.append("\r\n");
			for (const auto& member : members)
			{
				member.socket->send(msg);
			}

			if (members.size() == 1)
			{
				serv->channels.erase(e);
			}

			cd.channels.erase(channel_name);
		}
	}

	static void ircClientRecvLoop(IrcServer* serv, Socket& s) SOUP_EXCAL
	{
		s.recv([](Socket& s, std::string&& data, Capture&& cap) SOUP_EXCAL
		{
			IrcClientData& cd = s.custom_data.getStructFromMap(IrcClientData);
			cd.buffer.append(data);

			const auto serv = cap.get<IrcServer*>();

			// IRC spec says "\r\n", but in practice most servers support "\n" and some clients rely on that support.
			for (size_t i; i = cd.buffer.find('\n'), i != std::string::npos; cd.buffer.erase(0, i + 1))
			{
				auto line = cd.buffer.substr(0, i);
				if (line.back() == '\r')
				{
					line.pop_back();
				}
				serv->onClientLineReceived(s, line);
				if ((line.substr(0, 4) == "NICK" || line.substr(0, 4) == "nick") && line.length() > 5)
				{
					if (line.at(5) == '@' || line.at(5) == '+')
					{
						s.send(":Soup 432 Soup :Erroneus nickname\r\n");
					}
					else if (serv->getClient(line.substr(5)).isValid()
						|| serv->channels.find(line.substr(5)) != serv->channels.end()
						)
					{
						if (cd.nick.empty()
							&& cd.failednick.empty()
							)
						{
							cd.failednick = line.substr(5);
						}

						std::string msg = ":Soup 433 Soup ";
						msg.append(line.substr(5));
						msg.append(" is already in use\r\n");
						s.send(msg);
					}
					else if (cd.nick.empty())
					{
						cd.nick = line.substr(5);

						// Client doesn't update its nick locally on retry
						if (!cd.failednick.empty())
						{
							std::string msg(1, ':');
							msg.append(cd.failednick);
							msg.append(" NICK :");
							msg.append(cd.nick);
							msg.append("\r\n");
							s.send(msg);
							cd.failednick.clear();
						}
					}
					else
					{
						std::string msg(1, ':');
						msg.append(cd.nick);
						msg.append(" NICK :");
						msg.append(line.substr(5));
						msg.append("\r\n");

						/*std::unordered_set<Socket*> who_to_notify{};
						who_to_notify.emplace(&s);
						for (const auto& channel : cd.channels)
						{
							for (const auto& member : ircGetChannelMembers(channel.first))
							{
								who_to_notify.emplace(member.socket);
							}
						}
						for (const auto& s : who_to_notify)
						{
							s->send(msg);
						}*/

						// Clients might be DM'ing with each other but not share any channels, so we'll just let everyone know...
						for (const auto& w : serv->workers)
						{
							if (w->type == WORKER_TYPE_SOCKET
								&& static_cast<Socket*>(w.get())->custom_data.isStructInMap(IrcClientData)
								&& !static_cast<Socket*>(w.get())->custom_data.getStructFromMap(IrcClientData).nick.empty()
								)
							{
								static_cast<Socket*>(w.get())->send(msg);
							}
						}

						cd.nick = line.substr(5);
					}
				}
				else if (line.substr(0, 4) == "USER" && line.length() > 5)
				{
					auto sep = line.find(' ', 5);
					if (sep != std::string::npos)
					{
						//cd.name = line.substr(5, sep - 5);

						{
							std::string msg = ":Soup 001 ";
							msg.append(cd.nick);
							msg.append(" :Welcome ");
							msg.append(cd.nick);
							msg.push_back('!');
							msg.append(line.substr(5, sep - 5)); //msg.append(cd.name);
							msg.append("@Soup\r\n");
							s.send(msg);
						}

						{
							std::string msg = ":Soup 005 ";
							msg.append(cd.nick);
							msg.append(" LINELEN=512 :are supported by this server\r\n");
							s.send(msg);
						}

						// Using ERR_NOMOTD so HexChat shows this like the MOTD and proceeds to do autojoin.
						// Although this also means HexChat will start sending PING and disconnect if it doesn't get PONG.
						{
							std::string msg = ":Soup 422 ";
							msg.append(cd.nick);
							msg.append(" :Welcome! /join <channel> to talk to people.\r\n");
							s.send(msg);
						}
					}
				}
				else if ((line.substr(0, 4) == "JOIN" || line.substr(0, 4) == "join") && line.length() > 5)
				{
					for (const auto& channel_name : string::explode(line.substr(5), ','))
					{
						if (cd.nick.empty())
						{
							s.send(":Soup 451 Soup :Please select a unique nickname first.\r\n");
						}
						else if (cd.getMembership(channel_name))
						{
							s.send(":Soup 443 Soup :You're already in this channel.\r\n");
						}
						else if (serv->getClient(channel_name).isValid())
						{
							s.send(":Soup 405 Soup :This channel name is unavailable.\r\n");
						}
						else
						{
							std::string join_notify(1, ':');
							join_notify.append(cd.nick);
							join_notify.append(" JOIN :");
							join_notify.append(channel_name);
							join_notify.append("\r\n");
							s.send(join_notify);

							if (auto e = serv->channels.find(channel_name); e != serv->channels.end())
							{
								cd.channels.emplace(channel_name, IrcChannelMembershipData{ false });

								if (!e->second.topic.empty())
								{
									std::string msg = ":Soup 332 ";
									msg.append(cd.nick);
									msg.push_back(' ');
									msg.append(channel_name);
									msg.append(" :");
									msg.append(e->second.topic);
									msg.append("\r\n");
									s.send(msg);
								}
							}
							else
							{
								cd.channels.emplace(channel_name, IrcChannelMembershipData{ true });
								serv->channels.emplace(channel_name, IrcChannelData{});
							}

							std::string msg = ":Soup 353 ";
							msg.append(cd.nick);
							msg.append(" = ");
							msg.append(channel_name);
							msg.append(" :");
							for (const auto& member : serv->getChannelMembers(channel_name))
							{
								if (member.socket != &s)
								{
									member.socket->send(join_notify);
								}

								if (member.memberhip_data->op)
								{
									msg.push_back('@');
								}
								msg.append(member.data->nick);
								msg.push_back(' ');
							}
							msg.pop_back();
							msg.append("\r\n");
							s.send(msg);

							msg = ":Soup 366 ";
							msg.append(cd.nick);
							msg.push_back(' ');
							msg.append(channel_name);
							msg.append(" :End of /NAMES list\r\n");
							s.send(msg);
						}
					}
				}
				else if ((line.substr(0, 4) == "PART" || line.substr(0, 4) == "part") && line.length() > 5)
				{
					auto channel_name = string::explode(line.substr(5), ' ').at(0);
					if (cd.channels.find(channel_name) != cd.channels.end())
					{
						ircHandlePart(serv, cd, channel_name);
					}
				}
				else if ((line.substr(0, 5) == "TOPIC" || line.substr(0, 5) == "topic") && line.length() > 6)
				{
					auto arr = string::explode(line.substr(6), " :");
					SOUP_IF_UNLIKELY(arr.size() != 2)
					{
						return;
					}
					if (auto membership = cd.getMembership(arr.at(0)))
					{
						if (membership->op)
						{
							serv->channels.at(arr.at(0)).topic = arr.at(1);

							std::string msg(1, ':');
							msg.append(cd.nick);
							msg.push_back(' ');
							msg.append(line);
							msg.append("\r\n");
							ircChannelMsg(serv, arr.at(0), msg);
						}
						else
						{
							s.send(":Soup 482 Soup :You're not a channel operator.\r\n");
						}
					}
					else
					{
						s.send(":Soup 442 Soup :You're not in that channel.\r\n");
					}
				}
				else if (line.substr(0, 7) == "PRIVMSG" || line.substr(0, 6) == "NOTICE")
				{
					size_t cmdlen = (line.substr(0, 7) == "PRIVMSG" ? 7 : 6);
					size_t channel_name_begin = (cmdlen + 1);
					size_t channel_name_end = line.find(" :", channel_name_begin);
					SOUP_IF_UNLIKELY(channel_name_end == std::string::npos)
					{
						return;
					}
					auto channel_name = line.substr(channel_name_begin, channel_name_end - channel_name_begin);

					if (cd.getMembership(channel_name))
					{
						std::string msg(1, ':');
						msg.append(cd.nick);
						msg.push_back(' ');
						msg.append(line);
						msg.append("\r\n");
						ircChannelMsg(serv, channel_name, msg, &s);
					}
					else if (auto client = serv->getClient(channel_name); client.isValid())
					{
						std::string msg(1, ':');
						msg.append(cd.nick);
						msg.push_back(' ');
						msg.append(line);
						msg.append("\r\n");
						client.socket->send(msg);
					}
					else
					{
						s.send(":Soup 442 Soup :You're not in that channel.\r\n");
					}
				}
				else if (line.substr(0, 4) == "PING")
				{
					std::string msg = ":Soup PONG";
					msg.append(data.substr(4));
					s.send(msg);
				}
				else if (line.substr(0, 4) == "QUIT" || line.substr(0, 4) == "quit")
				{
					return;
				}
			}
			ircClientRecvLoop(serv, s);
		}, serv);
	}

	IrcServer::IrcServer()
		: srv([](Socket& s, ServerService&, Server& serv) SOUP_EXCAL
		{
			static_cast<IrcServer&>(serv).onClientConnected(s);
			ircClientRecvLoop(&static_cast<IrcServer&>(serv), s);
		})
	{
		on_work_done = [](Worker& w, Scheduler& s)
		{
			if (w.type == WORKER_TYPE_SOCKET
				&& static_cast<Socket&>(w).custom_data.isStructInMap(IrcClientData)
				)
			{
				const auto serv = &static_cast<IrcServer&>(s);
				serv->onClientDisconnected(static_cast<Socket&>(w));
				IrcClientData& cd = static_cast<Socket&>(w).custom_data.getStructFromMap(IrcClientData);
				while (!cd.channels.empty())
				{
					ircHandlePart(serv, cd, cd.channels.cbegin()->first);
				}
			}
		};
	}

	IrcClient IrcServer::getClient(const std::string& nick) const
	{
		for (const auto& w : this->workers)
		{
			if (w->type == WORKER_TYPE_SOCKET
				&& static_cast<Socket*>(w.get())->custom_data.isStructInMap(IrcClientData)
				)
			{
				IrcClientData& cd = static_cast<Socket*>(w.get())->custom_data.getStructFromMap(IrcClientData);
				if (cd.nick == nick)
				{
					return { static_cast<Socket*>(w.get()), &cd };
				}
			}
		}
		return {};
	}

	std::vector<IrcChannelMember> IrcServer::getChannelMembers(const std::string& channel_name) const
	{
		std::vector<IrcChannelMember> ret;
		for (const auto& w : this->workers)
		{
			if (w->type == WORKER_TYPE_SOCKET
				&& static_cast<Socket*>(w.get())->custom_data.isStructInMap(IrcClientData)
				)
			{
				IrcClientData& cd = static_cast<Socket*>(w.get())->custom_data.getStructFromMap(IrcClientData);
				if (auto membership = cd.getMembership(channel_name))
				{
					ret.emplace_back(IrcChannelMember{ { static_cast<Socket*>(w.get()), &cd }, membership });
				}
			}
		}
		return ret;
	}
}

#endif
