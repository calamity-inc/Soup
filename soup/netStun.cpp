#include "netStun.hpp"

#include "Scheduler.hpp"
#include "Socket.hpp"
#include "StringReader.hpp"
#include "StringWriter.hpp"

NAMESPACE_SOUP
{
	IpAddr netStun::queryBinding(const IpAddr& server_addr, uint16_t server_port, unsigned int timeout_ms)
	{
		IpAddr ret;

		StringWriter sw;
		uint32_t i = 0x0001 << 16; // Binding Request, Length 0
		sw.u32_be(i);
		i = 0x2112A442;
		sw.u32_be(i);
		i = 0;
		sw.u32_be(i);
		sw.u32_be(i);
		sw.u32_be(i);

		Scheduler sched;
		Socket& sock = *sched.addSocket();
		if (sock.udpClientSend(server_addr, server_port, sw.data))
		{
			sock.udpRecv([](Socket&, SocketAddr&&, std::string&& data, Capture&& cap)
			{
				StringReader sr(std::move(data));
				sr.skip(20);
				uint32_t i;
				while (sr.u32_be(i))
				{
					if ((i >> 16) == 0x20) // XOR-MAPPED-ADDRESS
					{
						sr.u32_be(i);
						if ((i >> 16) == 1) // IPv4
						{
							sr.u32_be(i);
							i ^= 0x2112A442;
							*cap.get<IpAddr*>() = IpAddr((native_u32_t)i);
						}
						else
						{
							uint32_t a, b, c, d;
							sr.u32_be(a);
							sr.u32_be(b);
							sr.u32_be(c);
							sr.u32_be(d);
							a ^= 0x2112A442;
							// remaining ints are XORed with transaction id, but we use all zeroes, so we're good.
							*cap.get<IpAddr*>() = IpAddr(a, b, c, d);
						}
					}
					else
					{
						sr.skip(i & 0xffff);
					}
					sr.skip(4 - (i % 4));
				}
			}, &ret);
			sched.runFor(timeout_ms);
		}

		return ret;
	}
}
