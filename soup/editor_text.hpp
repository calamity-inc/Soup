#pragma once

#include "conui_base.hpp"

#include "editor_file.hpp"

namespace soup
{
	struct editor_text : public conui_base
	{
		editor_file file;

		using conui_base::conui_base;

		void draw() const final;
	};
}
