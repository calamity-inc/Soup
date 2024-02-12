#pragma once

#include "netMeshMsgType.hpp"
#include "rsa.hpp"

namespace soup
{
	/*
	 * Soup Mesh Network is a simple way to remotely manage your servers.
	 * It is currently compatible with 'soup dnsserver', which will detect if the machine is configured to use the mesh network, and then listen for administrative commands.
	 *
	 * For management, you can use the web admin panel at http://soupmesh.net/ (source code at https://github.com/calamity-inc/soupmesh.net).
	 */
	struct netMesh
	{
		[[nodiscard]] static bool isEnabled() noexcept;

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
		[[nodiscard]] static MyConfig& getMyConfig() SOUP_EXCAL;

		static void addPeerLocally(Bigint n, uint32_t ip = 0) SOUP_EXCAL;

		[[nodiscard]] static uint32_t hashN(const Bigint& n);

#if false
		static void enableCryptoClient(Socket& s, const Bigint& remote_pub_n) SOUP_EXCAL;
#endif

		static bool bind(Server& serv);
	};
}
