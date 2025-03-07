#pragma once

#include "Server.hpp"
#if !SOUP_WASM

#include <string>
#include <unordered_map>

#include "ServerService.hpp"

NAMESPACE_SOUP
{
	struct IrcChannelMembershipData
	{
		bool op;
	};

	struct IrcClientData
	{
		std::string buffer;
		std::string nick;
		//std::string name;
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

	struct IrcServer : public Server
	{
		ServerService srv;
		std::unordered_map<std::string, IrcChannelData> channels;

		virtual void onClientConnected(Socket& s) {}
		virtual void onClientDisconnected(Socket& s) {}
		virtual void onClientLineReceived(Socket& s, const std::string& line) {}

		IrcServer();

		[[nodiscard]] IrcClient getClient(const std::string& nick) const;
		[[nodiscard]] std::vector<IrcChannelMember> getChannelMembers(const std::string& channel_name) const;
		void broadcast(const std::string& raw_msg) const;
	};
}

#endif
