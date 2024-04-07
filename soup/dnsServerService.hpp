#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "ServerServiceUdp.hpp"

#include <string>
#include <vector>

#include "dns_records.hpp"
#include "SharedPtr.hpp"

NAMESPACE_SOUP
{
	// serv.bindUdp(53, &dns_srv);
	class dnsServerService : public ServerServiceUdp
	{
	public:
		using on_query_t = std::vector<SharedPtr<dnsRecord>>(*)(const std::string& qname_joined, const dnsName& qname, const SocketAddr& sender, dnsType qtype);

		on_query_t on_query;

		dnsServerService(on_query_t on_query);

	private:
		void handle(Socket& s, SocketAddr&& addr, std::string&& data);
	};
}

#endif
