#pragma once

#include "json_node.hpp"

#include "derivable_map.hpp"

namespace soup
{
	struct json_object : public json_node
	{
		derivable_map<json_node, unique_ptr<json_node>> children{};

		explicit json_object() noexcept;
		explicit json_object(const char*& c) noexcept;

		[[nodiscard]] std::string encode() const final;
		[[nodiscard]] std::string encodePretty(const std::string& prefix = {}) const;

		[[nodiscard]] bool contains(const json_node& k);
		[[nodiscard]] bool contains(std::string k);
		[[nodiscard]] json_node& at(const json_node& k);
		[[nodiscard]] json_node& at(std::string k);

		void add(unique_ptr<json_node>&& k, unique_ptr<json_node>&& v);
		void add(std::string k, std::string v);
		void add(std::string k, int64_t v);
	};
}
