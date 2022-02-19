#pragma once

#include <istream>

#include "bigint.hpp"
#include "oid.hpp"

namespace soup
{
	struct asn1_sequence
	{
		std::string data;

		[[nodiscard]] static asn1_sequence fromBinary(const std::string& str);
		[[nodiscard]] static asn1_sequence fromBinary(std::istream& s);

		[[nodiscard]] size_t countChildren() const;

		[[nodiscard]] asn1_sequence getSeq(const size_t child_idx) const;
		[[nodiscard]] std::string getString(const size_t child_idx) const;
		[[nodiscard]] bigint getInt(const size_t child_idx) const;
		[[nodiscard]] oid getOid(const size_t child_idx) const;

		static void skipType(std::istream& s);
		[[nodiscard]] static size_t readLength(std::istream& s);
	};
}
