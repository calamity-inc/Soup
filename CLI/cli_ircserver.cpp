#include "cli.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

#include <Server.hpp>
#include <ServerService.hpp>
#include <Socket.hpp>
#include <string.hpp>

using namespace soup;

struct IrcChannelMembershipData
{
	bool op;
};

struct IrcClientData
{
	std::string buffer;
	std::string nick;
	std::string failednick;
	std::unordered_map<std::string, IrcChannelMembershipData> channels;

	[[nodiscard]] IrcChannelMembershipData* getMembership(const std::string& channel_name) noexcept
	{
		if (auto e = channels.find(channel_name); e != channels.end())
		{
			return &e->second;
		}
		return nullptr;
	}
};

struct IrcChannelData
{
	std::string topic;
};

struct IrcClient
{
	Socket* socket = nullptr;
	IrcClientData* data = nullptr;

	[[nodiscard]] bool isValid() const noexcept
	{
		return socket != nullptr;
	}
};

struct IrcChannelMember : public IrcClient
{
	IrcChannelMembershipData* memberhip_data;
};

static Server server;
static std::unordered_map<std::string, IrcChannelData> channels;

static IrcClient ircGetClient(const std::string& nick)
{
	for (const auto& w : server.workers)
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

static std::vector<IrcChannelMember> ircGetChannelMembers(const std::string& channel_name)
{
	std::vector<IrcChannelMember> ret;
	for (const auto& w : server.workers)
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

static void ircChannelMsg(const std::string& channel_name, const std::string& msg, Socket* exclude = nullptr)
{
	for (const auto& member : ircGetChannelMembers(channel_name))
	{
		if (member.socket != exclude)
		{
			member.socket->send(msg);
		}
	}
}

static void ircHandlePart(IrcClientData& cd, const std::string& channel_name)
{
	if (auto e = channels.find(channel_name); e != channels.end())
	{
		auto members = ircGetChannelMembers(channel_name);

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
			channels.erase(e);
		}

		cd.channels.erase(channel_name);
	}
}

static void ircClientRecvLoop(Socket& s) SOUP_EXCAL
{
	s.recv([](Socket& s, std::string&& data, Capture&&) SOUP_EXCAL
	{
		IrcClientData& cd = s.custom_data.getStructFromMap(IrcClientData);
		cd.buffer.append(data);

		// IRC spec says "\r\n", but in practice most servers support "\n" and some clients rely on that support.
		for (size_t i; i = cd.buffer.find('\n'), i != std::string::npos; cd.buffer.erase(0, i + 1))
		{
			auto line = cd.buffer.substr(0, i);
			if (line.back() == '\r')
			{
				line.pop_back();
			}
			std::cout << s.toString() << " | " << line << "\n";
			if ((line.substr(0, 4) == "NICK" || line.substr(0, 4) == "nick") && line.length() > 5)
			{
				if (line.at(5) == '@' || line.at(5) == '+')
				{
					s.send(":Soup 432 Soup :Erroneus nickname\r\n");
				}
				else if (ircGetClient(line.substr(5)).isValid()
					|| channels.find(line.substr(5)) != channels.end()
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

					// Using ERR_NOMOTD so HexChat shows this like the MOTD and proceeds to do autojoin.
					// Although this also means HexChat will start sending PING and disconnect if it doesn't get PONG.
					s.send(":Soup 422 Soup :Welcome! /join <channel> to talk to people.\r\n");
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
					for (const auto& w : server.workers)
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
					else if (ircGetClient(channel_name).isValid())
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

						if (auto e = channels.find(channel_name); e != channels.end())
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
							channels.emplace(channel_name, IrcChannelData{});
						}

						std::string msg = ":Soup 353 ";
						msg.append(cd.nick);
						msg.append(" = ");
						msg.append(channel_name);
						msg.append(" :");
						for (const auto& member : ircGetChannelMembers(channel_name))
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
					ircHandlePart(cd, channel_name);
				}
			}
			else if ((line.substr(0, 5) == "TOPIC" || line.substr(0, 5) == "topic") && line.length() > 6)
			{
				auto arr = string::explode(line.substr(6), " :");
				SOUP_IF_UNLIKELY (arr.size() != 2)
				{
					return;
				}
				if (auto membership = cd.getMembership(arr.at(0)))
				{
					if (membership->op)
					{
						channels.at(arr.at(0)).topic = arr.at(1);

						std::string msg(1, ':');
						msg.append(cd.nick);
						msg.push_back(' ');
						msg.append(line);
						msg.append("\r\n");
						ircChannelMsg(arr.at(0), msg);
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
					msg.push_back(' ');
					msg.append(line);
					msg.append("\r\n");
					ircChannelMsg(channel_name, msg, &s);
				}
				else if (auto client = ircGetClient(channel_name); client.isValid())
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
		ircClientRecvLoop(s);
	});
}

int cli_ircserver()
{
	server.on_work_done = [](Worker& w, Scheduler&)
	{
		if (w.type == WORKER_TYPE_SOCKET
			&& static_cast<Socket&>(w).custom_data.isStructInMap(IrcClientData)
			)
		{
			std::cout << static_cast<Socket&>(w).toString() << " has disconnected\n";
			IrcClientData& cd = static_cast<Socket&>(w).custom_data.getStructFromMap(IrcClientData);
			while (!cd.channels.empty())
			{
				ircHandlePart(cd, cd.channels.cbegin()->first);
			}
		}
	};
	ServerService serv([](Socket& s, ServerService&, Server&) SOUP_EXCAL
	{
		std::cout << s.toString() << " has connected\n";
		ircClientRecvLoop(s);
	});
	if (!server.bind(6667, &serv))
	{
		std::cout << "Failed to bind to port 6667\n";
		return 1;
	}
	std::cout << "Listening on port 6667\n";
	server.run();
	return 0;
}
