#pragma once

#include "Server.hpp"
#if !SOUP_WASM

#include <deque>
#include <string>
#include <unordered_map>

#include "Promise.hpp"
#include "ServerService.hpp"

NAMESPACE_SOUP
{
	struct IrcChannelMembershipData
	{
		bool op; // '@' prefix
		bool deaf = false;
	};

	struct IrcClientData
	{
		std::string buffer;
		std::string nick;
		std::string name;
		std::string failednick;
		std::unordered_map<std::string, IrcChannelMembershipData> channels;
		std::deque<std::string> pending_joins;
		UniquePtr<Promise<std::string>> promise;

		[[nodiscard]] IrcChannelMembershipData* getMembership(const std::string& channel_name) noexcept
		{
			if (auto e = channels.find(channel_name); e != channels.end())
			{
				return &e->second;
			}
			return nullptr;
		}

		[[nodiscard]] bool nickMatchesQuery(const std::string& query) const noexcept
		{
			if (query.size() != nick.size())
			{
				return false;
			}
			for (size_t i = 0; i != query.size(); ++i)
			{
				if (query[i] != nick[i] && query[i] != '?')
				{
					return false;
				}
			}
			return true;
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
		IrcChannelMembershipData* membership_data;
	};

	class IrcServer : public Server
	{
	public:
		ServerService srv;
		std::unordered_map<std::string, IrcChannelData> channels;

		virtual void onClientConnected(Socket& s) {}
		virtual void onClientDisconnected(Socket& s) {}
		virtual void onClientLineReceived(Socket& s, const std::string& line) {}
		virtual void canClientJoinChannel(Socket& s, const std::string& channel_name, Promise<std::string>& reject_reason_promise) { reject_reason_promise.fulfil({}); }
		virtual void onClientJoinedChannel(Socket& s, const std::string& channel_name, IrcChannelMembershipData& md) {}

		IrcServer();

		[[nodiscard]] IrcClient getClient(const std::string& nick) const;
		[[nodiscard]] IrcClient getClientWithWildcards(const std::string& query) const;
		[[nodiscard]] std::vector<IrcChannelMember> getChannelMembers(const std::string& channel_name) const;
		void broadcast(const std::string& raw_msg) const;

	protected:
		void clientRecvLoop(Socket& s) SOUP_EXCAL;
		void clientProcessPendingJoins(Socket& s) SOUP_EXCAL;
	};
}

#endif
