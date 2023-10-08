#pragma once

namespace soup
{
	template <typename T>
	struct UniqueListLink
	{
		T* next;
		T* prev;
	};
}
