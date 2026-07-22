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
			if (member.socket != exclude
				&& !member.membership_data->deaf
				)
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

	[[nodiscard]] static std::string ircEncodeLine(const std::vector<std::string>& params)
	{
		std::string str;
		if (!params.empty())
		{
			for (size_t i = 0; i != params.size() - 1; ++i)
			{
				str.append(params[i]);
				str.push_back(' ');
			}
			str.push_back(':');
			str.append(params.back());
			str.append("\r\n");
		}
		return str;
	}

	void IrcServer::clientRecvLoop(Socket& s) SOUP_EXCAL
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
						cd.name = line.substr(5, sep - 5);

						{
							std::string msg = ":Soup 001 ";
							msg.append(cd.nick);
							msg.append(" :Welcome ");
							msg.append(cd.nick);
							msg.push_back('!');
							msg.append(cd.name);
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
						else
						{
							cd.pending_joins.emplace_back(channel_name);
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
				else if ((line.substr(0, 4) == "KICK" || line.substr(0, 4) == "kick") && line.length() > 5)
				{
					auto arr = string::explode(line.substr(5), ' ');
					SOUP_IF_UNLIKELY(arr.size() != 2)
					{
						return;
					}
					if (auto membership = cd.getMembership(arr.at(0)))
					{
						if (membership->op)
						{
							if (auto target = serv->getClient(arr.at(1)); target.isValid())
							{
								ircHandlePart(serv, *target.data, arr.at(0));
							}
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
					SOUP_IF_UNLIKELY (channel_name_end == std::string::npos)
					{
						return;
					}
					auto channel_name = line.substr(channel_name_begin, channel_name_end - channel_name_begin);

					if (cd.getMembership(channel_name))
					{
						std::string msg(1, ':');
						msg.append(cd.nick);
						msg.push_back('!');
						msg.append(cd.name);
						msg.append("@Soup ");
						msg.append(line);
						msg.append("\r\n");
						ircChannelMsg(serv, channel_name, msg, &s);
					}
					else if (auto client = serv->getClient(channel_name); client.isValid())
					{
						SOUP_IF_UNLIKELY (cd.nick.empty())
						{
							s.send(":Soup 451 Soup :Please select a unique nickname first.\r\n");
						}
						else
						{
							std::string msg(1, ':');
							msg.append(cd.nick);
							msg.push_back('!');
							msg.append(cd.name);
							msg.append("@Soup ");
							msg.append(line);
							msg.append("\r\n");
							client.socket->send(msg);
						}
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
				else if (line.substr(0, 4) == "WHO " || line.substr(0, 4) == "who ")
				{
					if (line.c_str()[4] != '#')
					{
						auto sep = line.find(' ', 4);
						if (sep == std::string::npos)
						{
							sep = line.size();
						}

						bool bNarrow = false;
						bool bChannel = true;
						bool bUser = true;
						bool bHost = true;
						bool bServer = true;
						bool bNick = true;
						bool bFlags = true;
						bool bHops = true;
						bool bRealname = true;
						if (line.find('%', sep) != std::string::npos)
						{
							bNarrow = true;
							bChannel = line.find('c', sep) != std::string::npos;
							bUser = line.find('u', sep) != std::string::npos;
							bHost = line.find('h', sep) != std::string::npos;
							bServer = line.find('s', sep) != std::string::npos;
							bNick = line.find('n', sep) != std::string::npos;
							bFlags = line.find('f', sep) != std::string::npos;
							bHops = line.find('d', sep) != std::string::npos;
							bRealname = line.find('r', sep) != std::string::npos;
						}

						if (const auto target = serv->getClientWithWildcards(line.substr(4, sep - 4)); target.isValid())
						{
							std::vector<std::string> params = { ":Soup", bNarrow ? "354" : "352", cd.nick }; // bNarrow ? RPL_WHOSPCRPL : RPL_WHOREPLY
							if (bChannel)
							{
								params.emplace_back("*");
							}
							if (bUser)
							{
								params.emplace_back(target.data->name);
							}
							if (bHost)
							{
								params.emplace_back("Soup");
							}
							if (bServer)
							{
								params.emplace_back("Soup");
							}
							if (bNick)
							{
								params.emplace_back(target.data->nick);
							}
							if (bFlags)
							{
								params.emplace_back("H");
							}
							std::string realnamestr;
							if (bHops)
							{
								realnamestr = "0 ";
							}
							if (bRealname)
							{
								realnamestr.append(target.data->nick); // kinda incorrect currently because we don't track this
							}
							if (!realnamestr.empty())
							{
								params.emplace_back(std::move(realnamestr));
							}
							s.send(ircEncodeLine(params));
						}

						std::string msg = ":Soup 315 "; // RPL_ENDOFWHO
						msg.append(cd.nick);
						msg.push_back(' ');
						msg.append(line.substr(4, sep - 4));
						msg.append(" :End of /WHO list\r\n");
						s.send(msg);
					}
				}
				else if (line.substr(0, 6) == "WHOIS " || line.substr(0, 6) == "whois ")
				{
					auto sep = line.find(' ', 6);
					if (sep == std::string::npos)
					{
						sep = line.size();
					}

					if (const auto target = serv->getClient(line.substr(6, sep - 6)); target.isValid())
					{
						std::string msg = ":Soup 311 "; // RPL_WHOISUSER
						msg.append(cd.nick);
						msg.push_back(' ');
						msg.append(line.substr(6, sep - 6));
						msg.push_back(' ');
						msg.append(target.data->name); // <user>
						msg.append(" Soup * :"); // <host>
						msg.append(target.data->nick); // <realname> (kinda incorrect currently because we don't track this)
						msg.append("\r\n");
						s.send(msg);
					}

					std::string msg = ":Soup 318 "; // RPL_ENDOFWHOIS
					msg.append(cd.nick);
					msg.push_back(' ');
					msg.append(line.substr(6, sep - 6));
					msg.append(" :End of /WHOIS list\r\n");
					s.send(msg);
				}
				else if (line.substr(0, 4) == "QUIT" || line.substr(0, 4) == "quit")
				{
					return;
				}
			}

			if (cd.pending_joins.empty())
			{
				serv->clientRecvLoop(s);
			}
			else
			{
				serv->clientProcessPendingJoins(s);
			}
		}, this);
	}

	void IrcServer::clientProcessPendingJoins(Socket& s) SOUP_EXCAL
	{
		IrcClientData& cd = s.custom_data.getStructFromMapConst(IrcClientData);
		if (cd.pending_joins.empty())
		{
			return clientRecvLoop(s);
		}
		const auto& channel_name = cd.pending_joins.front();
		cd.promise = soup::make_unique<Promise<std::string>>();
		this->canClientJoinChannel(s, channel_name, *cd.promise);
		s.awaitPromiseCompletion(cd.promise.get(), [](Worker& w, Capture&& cap) SOUP_EXCAL
		{
			const auto serv = cap.get<IrcServer*>();
			Socket& s = static_cast<Socket&>(w);
			IrcClientData& cd = s.custom_data.getStructFromMapConst(IrcClientData);
			const auto& channel_name = cd.pending_joins.front();

			if (!cd.promise->getResult().empty())
			{
				std::string msg = ":Soup 474 ";
				msg.append(cd.nick);
				msg.push_back(' ');
				msg.append(channel_name);
				msg.append(" :");
				msg.append(cd.promise->getResult());
				msg.append("\r\n");
				s.send(msg);
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

				IrcChannelMembershipData* md;
				if (auto e = serv->channels.find(channel_name); e != serv->channels.end())
				{
					md = &cd.channels.emplace(channel_name, IrcChannelMembershipData{ false }).first->second;

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
					md = &cd.channels.emplace(channel_name, IrcChannelMembershipData{ true }).first->second;
					serv->channels.emplace(channel_name, IrcChannelData{});
				}
				serv->onClientJoinedChannel(s, channel_name, *md);

				if (md->op)
				{
					join_notify.append(":Soup MODE ");
					join_notify.append(channel_name);
					join_notify.append(" +o ");
					join_notify.append(cd.nick);
					join_notify.append("\r\n");
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

					if (member.membership_data->op)
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

				if (md->op)
				{
					std::string msg = ":Soup MODE ";
					join_notify.append(channel_name);
					join_notify.append(" +o ");
					join_notify.append(cd.nick);
					join_notify.append("\r\n");
				}
			}

			cd.pending_joins.pop_front();
			cd.promise.reset();
			serv->clientRecvLoop(s);
		}, this);
	}

	IrcServer::IrcServer()
		: srv([](Socket& s, ServerService&, Server& serv) SOUP_EXCAL
		{
			static_cast<IrcServer&>(serv).onClientConnected(s);
			static_cast<IrcServer&>(serv).clientRecvLoop(s);
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

	IrcClient IrcServer::getClientWithWildcards(const std::string& query) const
	{
		for (const auto& w : this->workers)
		{
			if (w->type == WORKER_TYPE_SOCKET
				&& static_cast<Socket*>(w.get())->custom_data.isStructInMap(IrcClientData)
				)
			{
				IrcClientData& cd = static_cast<Socket*>(w.get())->custom_data.getStructFromMap(IrcClientData);
				if (cd.nickMatchesQuery(query))
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

	void IrcServer::broadcast(const std::string& raw_msg) const
	{
		for (const auto& w : this->workers)
		{
			if (w->type == WORKER_TYPE_SOCKET
				&& static_cast<Socket*>(w.get())->custom_data.isStructInMap(IrcClientData)
				)
			{
				static_cast<Socket*>(w.get())->send(raw_msg);
			}
		}
	}
}

#endif
