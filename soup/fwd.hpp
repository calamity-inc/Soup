#pragma once

#include <cstdint>

namespace soup
{
	// chess
	struct chess_coordinate;

	// crypto
	struct keystore;

	// crypto.x509
	struct certchain;

	// data
	struct oid;

	// data.asn1
	struct asn1_sequence;

	// math
	class bigint;
	struct box_corners;
	class matrix;
	struct poly;
	struct ray;
	struct vector3;

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
	struct tls_client_hello;

	// os
	enum control_input : uint8_t;
	class module;
	enum mouse_button : uint8_t;

	// os.windows
	struct handle_raii;

	// task
	class promise_base;
	class scheduler;

	// ui.conui
	struct conui_app;
	struct conui_div;

	// ui.editor
	struct editor;
	struct editor_text;
}
