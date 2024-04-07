#include "lyoStyle.hpp"

#define FOR_EACH_PROP(f) \
f(display_block); \
f(margin_left); \
f(margin_right); \
f(margin_top); \
f(margin_bottom); \
f(font_size); \
f(color); \
f(background_color);

NAMESPACE_SOUP
{
	void lyoStyle::overrideWith(const lyoStyle& b) noexcept
	{
#define OVERRIDE(prop) if (b.prop) prop = b.prop;
		FOR_EACH_PROP(OVERRIDE)
	}

	void lyoStyle::propagateFromParent(const lyoStyle& b) noexcept
	{
#define OVERRIDE_FROM_PARENT(prop) if (!prop && b.prop) prop = b.prop;
		OVERRIDE_FROM_PARENT(font_size);
		OVERRIDE_FROM_PARENT(color);
	}

	void lyoStyle::reset() noexcept
	{
#define RESET(prop) prop = {};
		FOR_EACH_PROP(RESET)
	}
}
