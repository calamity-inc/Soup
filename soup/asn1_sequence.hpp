#pragma once

#include <istream>
#include <vector>

#include "fwd.hpp"

#include "asn1_element.hpp"

namespace soup
{
	struct asn1_sequence : public std::vector<asn1_element>
	{
		asn1_sequence();
		explicit asn1_sequence(std::string data);

		[[nodiscard]] static asn1_sequence fromBinary(const std::string& str);
		[[nodiscard]] static asn1_sequence fromBinary(std::istream& s);

		[[nodiscard]] size_t countChildren() const;

		[[nodiscard]] const asn1_identifier& getChildType(const size_t child_idx) const;
		[[nodiscard]] const std::string& getString(const size_t child_idx) const;
		[[nodiscard]] asn1_sequence getSeq(const size_t child_idx) const;
		[[nodiscard]] bigint getInt(const size_t child_idx) const;
		[[nodiscard]] oid getOid(const size_t child_idx) const;

		void addInt(const bigint& val);

		[[nodiscard]] std::string toDer() const;
		[[nodiscard]] std::string toString(const std::string& prefix = {}) const;

		static asn1_identifier readIdentifier(std::istream& s);
		[[nodiscard]] static size_t readLength(std::istream& s);
		[[nodiscard]] static std::string encodeLength(size_t len);
	};
}
