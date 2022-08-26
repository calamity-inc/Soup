#include "dnsUdpResolver.hpp"

#include "Scheduler.hpp"
#include "Socket.hpp"

namespace soup
{
	std::vector<dnsARecord> dnsUdpResolver::lookupA(const std::string& name) const
	{
		Socket sock;
		if (!sock.udpClientSend(server, getQueryA(name)))
		{
			return {};
		}
		Scheduler sched;
		std::string res{};
		sched.addSocket(std::move(sock)).udpRecv([](Socket&, SocketAddr&&, std::string&& data, Capture&& cap)
		{
			*cap.get<std::string*>() = std::move(data);
		}, &res);
		sched.runFor(timeout_ms);
		return parseResponseA(std::move(res));
	}

	std::vector<dnsAaaaRecord> dnsUdpResolver::lookupAAAA(const std::string& name) const
	{
		throw 0;
	}

	std::vector<dnsSrvRecord> dnsUdpResolver::lookupSRV(const std::string& name) const
	{
		throw 0;
	}

	std::vector<dnsTxtRecord> dnsUdpResolver::lookupTXT(const std::string& name) const
	{
		throw 0;
	}
}
