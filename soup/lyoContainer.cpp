#include "lyoContainer.hpp"

#include "lyoFlatDocument.hpp"
#include "lyoTextElement.hpp"
#include "unicode.hpp"

#if LYO_DEBUG_POS
#include "format.hpp"
#include "log.hpp"
#endif

NAMESPACE_SOUP
{
	lyoTextElement* lyoContainer::addText(const std::string& text)
	{
		return addText(unicode::utf8_to_utf32(text));
	}

	lyoTextElement* lyoContainer::addText(std::u32string text)
	{
		return static_cast<lyoTextElement*>(children.emplace_back(soup::make_unique<lyoTextElement>(this, std::move(text))).get());
	}

	lyoElement* lyoContainer::querySelector(const std::string& selector)
	{
		if (matchesSelector(selector))
		{
			return this;
		}
		for(const auto& node : children)
		{
			if (node->matchesSelector(selector))
			{
				return node.get();
			}
		}
		return nullptr;
	}

	std::vector<lyoElement*> lyoContainer::querySelectorAll(const std::string& selector)
	{
		std::vector<lyoElement*> res{};
		querySelectorAll(res, selector);
		return res;
	}

	void lyoContainer::querySelectorAll(std::vector<lyoElement*>& res, const std::string& selector)
	{
		lyoElement::querySelectorAll(res, selector);

		for (const auto& node : children)
		{
			node->querySelectorAll(res, selector);
		}
	}

	void lyoContainer::propagateStyle()
	{
		lyoElement::propagateStyle();
		propagateStyleToChildren();
	}

	void lyoContainer::propagateStyleToChildren() const
	{
		for (auto& elm : children)
		{
			elm->propagateStyle();
		}
	}

	void lyoContainer::populateFlatDocument(lyoFlatDocument& fdoc)
	{
		fdoc.elms.emplace_back(this);
		for (auto& elm : children)
		{
			elm->populateFlatDocument(fdoc);
		}
	}

	void lyoContainer::updateFlatSize()
	{
		if (parent != nullptr)
		{
			flat_width = parent->flat_width;
			flat_height = parent->flat_height;
		}

		for (auto& elm : children)
		{
			elm->updateFlatSize();
		}
	}

	void lyoContainer::updateFlatPos(unsigned int& x, unsigned int& y, unsigned int& wrap_y)
	{
#if LYO_DEBUG_POS
		logWriteLine(format("lyoContainer({})::updateFlatPos - Start: {}, {}", tag_name, x, y));
#endif

		// Process margins
		lyoElement::updateFlatPos(x, y, wrap_y);

#if LYO_DEBUG_POS
		logWriteLine(format("lyoContainer({})::updateFlatPos - Margin: {}, {}", tag_name, x, y));
#endif

		// Check line wrap
		if (parent != nullptr
			&& style.display_block
			)
		{
			wrapLine(x, y, wrap_y);

#if LYO_DEBUG_POS
			logWriteLine(format("lyoContainer({})::updateFlatPos - Wrap: {}, {}", tag_name, x, y));
#endif
		}

		// Done positioning this element
		setFlatPos(x, y);

		// Position children
		for (auto& elm : children)
		{
			elm->updateFlatPos(x, y, wrap_y);
		}

#if LYO_DEBUG_POS
		logWriteLine(format("lyoContainer({})::updateFlatPos - End: {}, {}", tag_name, x, y));
#endif
	}

	void lyoContainer::narrowFlatSize()
	{
		for (auto& elm : children)
		{
			elm->narrowFlatSize();
		}

		if (parent != nullptr)
		{
			flat_width = 0;
			flat_height = 0;
			for (auto& elm : children)
			{
				auto width = (elm->flat_x - flat_x) + elm->flat_width;
				auto height = (elm->flat_y - flat_y) + elm->flat_height;
				if (width > flat_width
					&& width <= parent->flat_width
					)
				{
					flat_width = width;
				}
				if (height > flat_height
					&& height <= parent->flat_width
					)
				{
					flat_height = height;
				}
			}
		}
	}
}
