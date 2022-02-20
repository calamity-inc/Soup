#pragma once

namespace soup
{
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

	// net.dns
	struct dns_a;
	struct dns_aaaa;
	struct dns_srv;
	struct dns_txt;

	// os
	class module;

	// os.windows
	struct handle_raii;
}
