#include "netMeshService.hpp"
#if !SOUP_WASM

#include <iostream>

#include "netMesh.hpp"
#include "sha256.hpp"
#include "Socket.hpp"
#include "StringReader.hpp"

namespace soup
{
	void netMeshService::onTunnelEstablished(Socket& s, ServerService&, Server&) SOUP_EXCAL
	{
		s.recv([](Socket& s, std::string&& data, Capture&&) SOUP_EXCAL
		{
			StringReader sr(std::move(data));
			uint8_t msg_type;
			if (sr.u8(msg_type))
			{
				if (msg_type == MESH_MSG_LINK)
				{
					if (g_mesh_service.link_passnum != 0)
					{
						if (uint64_t passnum; sr.u64(passnum) && passnum == g_mesh_service.link_passnum)
						{
							if (Bigint remote_pub_n; sr.bigint_lp_u64_dyn(remote_pub_n))
							{
								s.send(std::string(1, MESH_MSG_OK));
								netMesh::addPeerLocally(std::move(remote_pub_n));
								std::cout << "Successfully linked.\n";
								exit(0);
							}
						}
					}
				}
				else if (msg_type == MESH_MSG_OK)
				{
					// Not currently used in this way.
				}
				else if (msg_type == MESH_MSG_DNS_ADD_RECORD)
				{
					if (g_mesh_service.app_msg_handler)
					{
						if (std::string data; sr.str_lp_u64_dyn(data))
						{
							if (uint32_t n_hash; sr.u32(n_hash))
							{
								if (Bigint signature; sr.bigint_lp_u64_dyn(signature))
								{
									if (auto peer = netMesh::getMyConfig().findPeer(n_hash, s.peer.ip.getV4NativeEndian()))
									{
										if (peer->getPublicKey().verify<sha256>(data, signature))
										{
											if (g_mesh_service.app_msg_handler((netMeshMsgType)msg_type, std::move(data)))
											{
												s.send(std::string(1, MESH_MSG_OK));
											}
										}
									}
								}
							}
						}
					}
				}
				else
				{
					// Unknown message type.
				}
			}
		});
	}
}

#endif
