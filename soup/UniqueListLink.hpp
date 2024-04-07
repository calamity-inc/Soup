#pragma once

#include "base.hpp"

NAMESPACE_SOUP
{
	template <typename T>
	struct UniqueListLink
	{
		T* next;
		T* prev;
	};
}
