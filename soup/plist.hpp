#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "fwd.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct PlistNode
	{
		enum Type : uint8_t
		{
			DICT,
			ARRAY,
			STRING,
		};

		const Type type;

		explicit PlistNode(Type type)
			: type(type)
		{
		}

		virtual ~PlistNode() = default;

		[[nodiscard]] std::string toString(const std::string& prefix = {}) const noexcept;

		// Type checks.
		[[nodiscard]] bool isDict() const noexcept;
		[[nodiscard]] bool isArray() const noexcept;
		[[nodiscard]] bool isString() const noexcept;

		// Type casts; will throw if node is of different type.
		[[nodiscard]] PlistDict& asDict();
		[[nodiscard]] PlistArray& asArray();
		[[nodiscard]] PlistString& asString();
		[[nodiscard]] const PlistDict& asDict() const;
		[[nodiscard]] const PlistArray& asArray() const;
		[[nodiscard]] const PlistString& asString() const;
	};

	struct PlistDict : public PlistNode
	{
		std::unordered_map<std::string, UniquePtr<PlistNode>> children{};

		PlistDict()
			: PlistNode(DICT)
		{
		}
	};

	struct PlistArray : public PlistNode
	{
		std::vector<UniquePtr<PlistNode>> children{};

		PlistArray()
			: PlistNode(ARRAY)
		{
		}
	};

	struct PlistString : public PlistNode
	{
		std::string data;

		PlistString()
			: PlistNode(STRING)
		{
		}
	};

	struct plist
	{
		// For the "plist" tag -- plist::parse(*xml::parse(data)->findTag("plist"))
		[[nodiscard]] static std::vector<UniquePtr<PlistNode>> parse(const XmlTag& tag);

		[[nodiscard]] static UniquePtr<PlistNode> parseTag(const XmlTag& tag);
	};
}
