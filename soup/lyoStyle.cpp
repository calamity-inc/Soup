#include "lyoStyle.hpp"

#define FOR_EACH_PROP(f) \
f(display_inline); \
f(margin_left); \
f(margin_right); \
f(margin_top); \
f(margin_bottom); \
f(font_size); \
f(color); \
f(background_color);

namespace soup
{
	void lyoStyle::overrideWith(const lyoStyle& b) noexcept
	{
#define OVERRIDE(prop) if (b.prop) prop = b.prop;
		FOR_EACH_PROP(OVERRIDE)
	}

	void lyoStyle::reset() noexcept
	{
#define RESET(prop) prop = {};
		FOR_EACH_PROP(RESET)
	}
}
