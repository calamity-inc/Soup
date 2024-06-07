#include "netIntrospectTask.hpp"
#if !SOUP_WASM

NAMESPACE_SOUP
{
	void netIntrospectTask::LookupTaskPair::appendDiagnostic(std::string & str) const SOUP_EXCAL
	{
		str += "- Asking for an IPv4: ";
		if (!ipv4_task->result)
		{
			str += "Did not get a reply.";
		}
		else if (ipv4_task->result->size() != 1)
		{
			str += "Got a reply, but it's been tampered with: got ";
			str += std::to_string(ipv4_task->result->size());
			str += " RRs, should've been 1.";
		}
		else if (ipv4_task->result->at(0)->type != DNS_A)
		{
			str += "Got a reply, but it's been tampered with: did not get an A record as expected.";
		}
		else if (static_cast<dnsARecord*>(ipv4_task->result->at(0).get())->data != SOUP_IPV4_NWE(1, 3, 3, 7))
		{
			str += "Got a reply, but it's been tampered with: IP address has been replaced with ";
			str += IpAddr(static_cast<dnsARecord*>(ipv4_task->result->at(0).get())->data).toString4();
		}
		else
		{
			str += "OK!";
		}

		str += "\n- Asking for an IPv6: ";
		if (!ipv6_task->result)
		{
			str += "Did not get a reply.";
		}
		else if (ipv6_task->result->size() != 1)
		{
			str += "Got a reply, but it's been tampered with: got ";
			str += std::to_string(ipv6_task->result->size());
			str += " RRs, should've been 1.";
		}
		else if (ipv6_task->result->at(0)->type != DNS_AAAA)
		{
			str += "Got a reply, but it's been tampered with: did not get an AAAA record as expected.";
		}
		else if (static_cast<dnsAaaaRecord*>(ipv6_task->result->at(0).get())->data != IpAddr(0, 0, 0, 0, 0, 0, 0, 0xCAFE))
		{
			str += "Got a reply, but it's been tampered with: IP address has been replaced with ";
			str += static_cast<dnsAaaaRecord*>(ipv6_task->result->at(0).get())->data.toString6();
		}
		else
		{
			str += "OK!";
		}
	}
}

#endif
