#pragma once

#include "netMeshMsgType.hpp"
#include "rsa.hpp"

namespace soup
{
	/*
	 * Soup Mesh Network is a simple way to remotely manage your servers.
	 * It is currently compatible with 'soup dnsserver', which will detect if the machine is configured to use the mesh network, and then listen for administrative commands.
	 *
	 * To set up a server with the mesh network, run 'soup mesh link' on that machine. Then, run the provided command on your personal computer.
	 * This will allow you to manage the mesh network without becoming a part of it.
	 */
	struct netMesh
	{
		[[nodiscard]] static bool isEnabled();

		struct Peer
		{
			uint32_t n_hash;
			uint32_t ip;
			Bigint n;

			[[nodiscard]] bool isRepresentedByIp(uint32_t ip) const noexcept
			{
				return this->ip == 0
					|| this->ip == ip
					;
			}

			[[nodiscard]] RsaPublicKey getPublicKey() const
			{
				return RsaPublicKey(n);
			}
		};

		struct MyConfig
		{
			RsaKeypair kp;
			std::vector<Peer> peers;

			[[nodiscard]] Peer* findPeer(uint32_t ip) noexcept;
			[[nodiscard]] const Peer* findPeerByPublicKey(uint32_t n_hash) const noexcept;
			[[nodiscard]] const Peer* findPeer(uint32_t n_hash, uint32_t ip) const noexcept;
		};
		[[nodiscard]] static MyConfig& getMyConfig();

		static void addPeerLocally(Bigint n, uint32_t ip = 0);

		static void enableCryptoClient(Socket& s, Bigint remote_pub_n, void(*callback)(Socket&, Capture&&), Capture&& cap);
		static void sendAppMessage(Socket& s, netMeshMsgType msg_type, const std::string& data);
	};
}
