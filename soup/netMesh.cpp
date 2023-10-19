#include "netMesh.hpp"

#include "base.hpp"
#if !SOUP_WASM

#include <iostream>

#include "FileReader.hpp"
#include "FileWriter.hpp"
#include "fnv.hpp"
#include "os.hpp"
#include "sha256.hpp"
#include "Socket.hpp"
#include "StringWriter.hpp"

namespace soup
{
	using Peer = netMesh::Peer;
	using MyConfig = netMesh::MyConfig;

	[[nodiscard]] static std::filesystem::path getDataPath()
	{
		auto path = os::getProgramData();
		path /= "Calamity, Inc";
		path /= "Soup";
		path /= "Mesh Network";
		return path;
	}

	bool netMesh::isEnabled()
	{
		return std::filesystem::is_regular_file(getDataPath() / "keypair.bin")
			&& !getMyConfig().peers.empty()
			;
	}

	static MyConfig s_my_config;

	MyConfig& netMesh::getMyConfig()
	{
		if (s_my_config.kp.n.isZero())
		{
			const auto path = getDataPath();
			if (!std::filesystem::is_directory(path))
			{
				std::filesystem::create_directories(path);
			}

			const auto kp_path = path / "keypair.bin";
			if (std::filesystem::is_regular_file(kp_path))
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
			if (std::filesystem::is_regular_file(peers_path))
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

	void netMesh::addPeerLocally(Bigint n, uint32_t ip)
	{
		s_my_config.peers.emplace_back(Peer{ fnv1a_32(n.toBinary()), ip, std::move(n) });

		FileWriter fw(getDataPath() / "peers.bin");
		{ uint8_t version = 0; fw.u8(version); }
		fw.u64_dyn(s_my_config.peers.size());
		for (auto& peer : s_my_config.peers)
		{
			fw.bigint_lp_u64_dyn(peer.n);
			fw.u32(peer.ip);
		}
	}

	const Peer* netMesh::MyConfig::findPeer(uint32_t ip) const noexcept
	{
		for (const auto& peer : peers)
		{
			if (peer.ip == ip)
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

	void netMesh::Peer::sendAppMessage(Socket& s, netMeshMsgType msg_type, std::string&& data) const
	{
		std::string to_encrypt = std::move(data);
		to_encrypt.push_back((char)msg_type);

		auto encrypted = getPublicKey().encryptPkcs1(to_encrypt);

		std::string tbs = encrypted.toBinary();
		tbs.push_back((char)msg_type);

		auto signature = netMesh::getMyConfig().kp.getPrivate().sign<sha256>(tbs);

		auto n_hash = fnv1a_32(netMesh::getMyConfig().kp.n.toBinary());

		StringWriter sw;
		{ auto msg_type_u8 = (uint8_t)msg_type; sw.u8(msg_type_u8); }
		sw.bigint_lp_u64_dyn(encrypted);
		sw.u32(n_hash);
		sw.bigint_lp_u64_dyn(signature);

		s.send(sw.data);
	}
}

#endif