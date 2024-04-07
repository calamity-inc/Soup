#pragma once

#include "ConuiBase.hpp"

#include "EditorFile.hpp"

NAMESPACE_SOUP
{
	struct EditorText : public ConuiBase
	{
		EditorFile file;

		using ConuiBase::ConuiBase;

		void draw() const final;
	};
}
