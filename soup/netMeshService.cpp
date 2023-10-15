#include "netMeshService.hpp"

#include <iostream>

#include "netMesh.hpp"
#include "Scheduler.hpp"
#include "sha256.hpp"
#include "Socket.hpp"
#include "StringReader.hpp"
#include "StringRefReader.hpp"

namespace soup
{
	void netMeshService::onTunnelEstablished(Socket& s, ServerService&, Server&)
	{
		s.recv([](Socket& s, std::string&& data, Capture&&)
		{
			StringReader sr(std::move(data));
			uint8_t msg_type;
			if (sr.u8(msg_type))
			{
				if (msg_type == MESH_MSG_LINK)
				{
					if (g_mesh_service.link_passnum != 0)
					{
						if (Bigint encrypted; sr.bigint_lp_u64_dyn(encrypted))
						{
							auto decrypted = netMesh::getMyConfig().kp.getPrivate().decryptPkcs1(encrypted);
							StringRefReader dr(decrypted);
							if (uint64_t passnum; dr.u64(passnum) && passnum == g_mesh_service.link_passnum)
							{
								if (std::string remote_pub_thumbprint; dr.str_lp_u64_dyn(remote_pub_thumbprint))
								{
									if (Bigint remote_pub_n; sr.bigint_lp_u64_dyn(remote_pub_n))
									{
										RsaPublicKey remote_pub(std::move(remote_pub_n));
										if (remote_pub.getJwkThumbprint() == remote_pub_thumbprint)
										{
											s.send(std::string(1, MESH_MSG_OK));
											netMesh::addPeerLocally(std::move(remote_pub.n));
											std::cout << "Successfully linked.\n";
											exit(0);
										}
									}
								}
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
						if (Bigint encrypted; sr.bigint_lp_u64_dyn(encrypted))
						{
							if (uint32_t n_hash; sr.u32(n_hash))
							{
								if (auto peer = netMesh::getMyConfig().findPeer(n_hash, s.peer.ip.getV4NativeEndian()))
								{
									if (Bigint signature; sr.bigint_lp_u64_dyn(signature))
									{
										// BUG: Replay attacks are possible.
										std::string tbs = encrypted.toBinary();
										tbs.push_back(msg_type);
										if (peer->getPublicKey().verify<sha256>(tbs, signature))
										{
											auto decrypted = netMesh::getMyConfig().kp.getPrivate().decryptPkcs1(encrypted);
											if (decrypted.back() == msg_type)
											{
												decrypted.pop_back();
												if (g_mesh_service.app_msg_handler((netMeshMsgType)msg_type, std::move(decrypted)))
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
				}
				else
				{
					// Unknown message type.
				}
			}
		});
	}
}
