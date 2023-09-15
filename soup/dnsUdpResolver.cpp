#include "dnsUdpResolver.hpp"

#if !SOUP_WASM

#include "Scheduler.hpp"
#include "Socket.hpp"

namespace soup
{
	std::vector<UniquePtr<dnsRecord>> dnsUdpResolver::lookup(dnsType qtype, const std::string& name) const
	{
		{
			std::vector<UniquePtr<dnsRecord>> res;
			if (checkBuiltinResult(res, qtype, name))
			{
				return res;
			}
		}

		Socket sock;
		if (!sock.udpClientSend(server, getQuery(qtype, name)))
		{
			return {};
		}
		Scheduler sched;
		std::string res{};
		sched.addSocket(std::move(sock))->udpRecv([](Socket&, SocketAddr&&, std::string&& data, Capture&& cap)
		{
			*cap.get<std::string*>() = std::move(data);
		}, &res);
		sched.runFor(timeout_ms);
		return parseResponse(std::move(res));
	}
}

#endif
