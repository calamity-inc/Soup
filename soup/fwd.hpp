#pragma once

namespace soup
{
	// io.asn1
	struct asn1_sequence;

	// io
	struct oid;

	// math
	class bigint;
	struct box_corners;
	struct poly;
	struct ray;

	// mem
	struct alloc_raii_remote;
	struct pattern;
	struct pattern_compile_time_with_opt_bytes_base;
	class pointer;
	class range;
	struct region_virtual;

	// net
	class addr_ip;
	class socket;

	// net.dns
	struct dns_a;
	struct dns_aaaa;
	struct dns_srv;
	struct dns_txt;

	// net.tls
	class socket_tls_handshaker;
	struct socket_tls_server_rsa_data;

	// os
	class module;

	// os.windows
	struct handle_raii;

	// task
	class promise_base;
	class scheduler;
}
