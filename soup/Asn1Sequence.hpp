#pragma once

#include <istream>
#include <vector>

#include "fwd.hpp"

#include "Asn1Element.hpp"

namespace soup
{
	struct Asn1Sequence : public std::vector<Asn1Element>
	{
		Asn1Sequence();
		explicit Asn1Sequence(std::string data);

		[[nodiscard]] static Asn1Sequence fromBinary(const std::string& str);
		[[nodiscard]] static Asn1Sequence fromBinary(std::istream& s);

		[[nodiscard]] size_t countChildren() const;

		[[nodiscard]] const Asn1Identifier& getChildType(const size_t child_idx) const;
		[[nodiscard]] const std::string& getString(const size_t child_idx) const;
		[[nodiscard]] Asn1Sequence getSeq(const size_t child_idx) const;
		[[nodiscard]] Bigint getInt(const size_t child_idx) const;
		[[nodiscard]] Oid getOid(const size_t child_idx) const;

		void addInt(const Bigint& val);

		[[nodiscard]] std::string toDer() const;
		[[nodiscard]] std::string toString(const std::string& prefix = {}) const;

		static Asn1Identifier readIdentifier(std::istream& s);
		[[nodiscard]] static size_t readLength(std::istream& s);
		[[nodiscard]] static std::string encodeLength(size_t len);
	};
}
