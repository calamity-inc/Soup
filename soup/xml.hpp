#pragma once

#include "TreeNode.hpp"

#include <string>
#include <vector>

#include "UniquePtr.hpp"

namespace soup
{
	struct XmlNode : public TreeNode
	{
		const bool is_text;

		constexpr XmlNode(bool is_text) noexcept
			: is_text(is_text)
		{
		}
	};

	struct XmlTag : public XmlNode
	{
		std::string name{};
		std::vector<UniquePtr<XmlNode>> children{};
		std::vector<std::pair<std::string, std::string>> attributes{};

		XmlTag() noexcept
			: XmlNode(false)
		{
		}

		[[nodiscard]] std::string encode() const noexcept;
	};

	struct XmlText : public XmlNode
	{
		std::string contents{};

		XmlText() noexcept
			: XmlNode(true)
		{
		}

		XmlText(std::string&& contents) noexcept
			: XmlNode(true), contents(std::move(contents))
		{
		}
	};

	class xml
	{
	public:
		[[nodiscard]] static UniquePtr<XmlTag> parse(const std::string& xml);
	private:
		[[nodiscard]] static UniquePtr<XmlTag> parse(const std::string& xml, std::string::const_iterator& i);
	};
}
