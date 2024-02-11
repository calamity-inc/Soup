#include "netMesh.hpp"

#include "base.hpp"
#if !SOUP_WASM

#include <iostream>

#include "FileReader.hpp"
#include "FileWriter.hpp"
#include "fnv.hpp"
#include "netConfig.hpp"
#include "netMeshService.hpp"
#include "os.hpp"
#include "Server.hpp"
#include "sha256.hpp"
#include "Socket.hpp"
#include "StringWriter.hpp"
#include "TlsServerRsaData.hpp"
#include "X509Certchain.hpp"

namespace soup
{
	using Peer = netMesh::Peer;
	using MyConfig = netMesh::MyConfig;

	[[nodiscard]] static std::filesystem::path getDataPath() noexcept
	{
		auto path = os::getProgramData();
		path /= "Calamity, Inc";
		path /= "Soup";
		path /= "Mesh Network";
		return path;
	}

	bool netMesh::isEnabled() noexcept
	{
		std::error_code ec;
		return std::filesystem::is_regular_file(getDataPath() / "keypair.bin", ec)
			&& !getMyConfig().peers.empty()
			;
	}

	static MyConfig s_my_config;

	MyConfig& netMesh::getMyConfig() SOUP_EXCAL
	{
		std::error_code ec;
		if (s_my_config.kp.n.isZero())
		{
			const auto path = getDataPath();
			if (!std::filesystem::is_directory(path, ec))
			{
				std::filesystem::create_directories(path, ec);
			}

			const auto kp_path = path / "keypair.bin";
			if (std::filesystem::is_regular_file(kp_path, ec))
			{
				FileReader fr(kp_path);
				Bigint p, q;
				if (fr.bigint_lp_u64_dyn(p) && fr.bigint_lp_u64_dyn(q))
				{
					s_my_config.kp = RsaKeypair(std::move(p), std::move(q));
				}
			}
			if (s_my_config.kp.n.isZero())
			{
				std::cout << "One-time setup: Generating keypair for this machine...\n";
				s_my_config.kp = RsaKeypair::generate(1536, true);
				FileWriter fw(kp_path);
				fw.bigint_lp_u64_dyn(s_my_config.kp.p);
				fw.bigint_lp_u64_dyn(s_my_config.kp.q);
			}

			const auto peers_path = path / "peers.bin";
			if (std::filesystem::is_regular_file(peers_path, ec))
			{
				FileReader fr(peers_path);
				fr.skip(1); // version
				if (uint64_t num_peers; fr.u64_dyn(num_peers))
				{
					while (num_peers--)
					{
						Peer& peer = s_my_config.peers.emplace_back();
						fr.bigint_lp_u64_dyn(peer.n);
						fr.u32(peer.ip);
						peer.n_hash = fnv1a_32(peer.n.toBinary());
					}
				}
			}
		}
		return s_my_config;
	}

	void netMesh::addPeerLocally(Bigint n, uint32_t ip) SOUP_EXCAL
	{
		const auto n_hash = fnv1a_32(n.toBinary());

		if (ip == 0) // Administrative device?
		{
			if (s_my_config.findPeerByPublicKey(n_hash))
			{
				return; // Already known, no need to add it again.
			}
		}
		else
		{
			if (auto peer = s_my_config.findPeer(ip))
			{
				// Already know this peer; update key but don't create a duplicate entry.
				peer->n_hash = n_hash;
				peer->n = std::move(n);
				goto _save_peers;
			}
		}

		s_my_config.peers.emplace_back(Peer{ n_hash, ip, std::move(n) });

	_save_peers:
		FileWriter fw(getDataPath() / "peers.bin");
		{ uint8_t version = 0; fw.u8(version); }
		fw.u64_dyn(s_my_config.peers.size());
		for (auto& peer : s_my_config.peers)
		{
			fw.bigint_lp_u64_dyn(peer.n);
			fw.u32(peer.ip);
		}
	}

	struct netMeshTlsClientData
	{
		Bigint remote_pub_n;
	};

	struct netMeshEnableCryptoClientCapture
	{
		void(*callback)(Socket&, Capture&&) SOUP_EXCAL;
		Capture cap;
	};

	void netMesh::enableCryptoClient(Socket& s, Bigint remote_pub_n, void(*callback)(Socket&, Capture&&) SOUP_EXCAL, Capture&& cap) SOUP_EXCAL
	{
		s.custom_data.getStructFromMap(netMeshTlsClientData).remote_pub_n = std::move(remote_pub_n);
		netConfig::get().certchain_validator = [](const X509Certchain& chain, const std::string&, StructMap& custom_data)
		{
			return chain.certs.size() == 1
				&& chain.certs.at(0).isRsa()
				&& custom_data.getStructFromMapConst(netMeshTlsClientData).remote_pub_n == chain.certs.at(0).getRsaPublicKey().n
				;
		};
		s.enableCryptoClient({}, [](Socket& s, Capture&& _cap) SOUP_EXCAL
		{
			netMeshEnableCryptoClientCapture& cap = _cap.get<netMeshEnableCryptoClientCapture>();
			cap.callback(s, std::move(cap.cap));
		}, netMeshEnableCryptoClientCapture{ callback, std::move(cap) });
	}

	void netMesh::sendAppMessage(Socket& s, netMeshMsgType msg_type, const std::string& data) SOUP_EXCAL
	{
		auto n_hash = fnv1a_32(netMesh::getMyConfig().kp.n.toBinary());
		auto signature = netMesh::getMyConfig().kp.getPrivate().sign<sha256>(data);

		StringWriter sw;
		{ auto msg_type_u8 = (uint8_t)msg_type; sw.u8(msg_type_u8); }
		sw.str_lp_u64_dyn(data);
		sw.u32(n_hash);
		sw.bigint_lp_u64_dyn(signature);

		s.send(sw.data);
	}

	static void cert_selector(TlsServerRsaData& out, const std::string&) SOUP_EXCAL
	{
		X509Certificate cert;
		cert.setRsaPublicKey(netMesh::getMyConfig().kp.getPublic());
		out.der_encoded_certchain = { cert.toDer() };
		out.private_key = netMesh::getMyConfig().kp.getPrivate();
	}

	bool netMesh::bind(Server& serv)
	{
		return serv.bindCrypto(7106, &g_mesh_service, &cert_selector);
	}

	Peer* netMesh::MyConfig::findPeer(uint32_t ip) noexcept
	{
		for (auto& peer : peers)
		{
			if (peer.ip == ip)
			{
				return &peer;
			}
		}
		return nullptr;
	}

	const Peer* netMesh::MyConfig::findPeerByPublicKey(uint32_t n_hash) const noexcept
	{
		for (const auto& peer : peers)
		{
			if (peer.n_hash == n_hash)
			{
				return &peer;
			}
		}
		return nullptr;
	}

	const Peer* netMesh::MyConfig::findPeer(uint32_t n_hash, uint32_t ip) const noexcept
	{
		for (const auto& peer : peers)
		{
			if (peer.n_hash == n_hash
				&& peer.isRepresentedByIp(ip)
				)
			{
				return &peer;
			}
		}
		return nullptr;
	}
}

#endif
