#pragma once

#include "fwd.hpp"

#include "Capture.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	struct drAdaptor
	{
		using func_t = UniquePtr<drData>(*)(const drData&, const Capture&);

		const char* name;
		func_t func;
		Capture cap;

		drAdaptor(const char* name, func_t func)
			: name(name), func(func)
		{
		}

		drAdaptor(const char* name, func_t func, Capture&& cap)
			: name(name), func(func), cap(std::move(cap))
		{
		}

		[[nodiscard]] UniquePtr<drData> operator()(const drData& data) const
		{
			return func(data, cap);
		}
	};
}
