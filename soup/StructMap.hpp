#pragma once

#include <cstdint>
#include <unordered_map>

#include "Capture.hpp"
#include "joaat.hpp"

namespace soup
{
	struct StructMap : public std::unordered_map<uint32_t, Capture>
	{
		[[nodiscard]] bool containsImpl(uint32_t id) const noexcept
		{
			return find(id) != end();
		}

		template <typename T>
		[[nodiscard]] T& getImpl(uint32_t id)
		{
			auto e = find(id);
			if (e != end())
			{
				return e->second.get<T>();
			}
			return emplace(id, T{}).first->second.template get<T>();
		}

		void removeImpl(uint32_t id)
		{
			if (auto e = find(id); e != end())
			{
				erase(e);
			}
		}

#define isStructInMap(T) containsImpl(::soup::joaat::hash(#T))
#define addStructToMap(T, inst) emplace(::soup::joaat::hash(#T), inst); static_assert(std::is_same_v<T, decltype(inst)>)
#define getStructFromMap(T) getImpl<T>(::soup::joaat::hash(#T))
#define getStructFromMapConst(T) at(::soup::joaat::hash(#T)).get<T>()
#define removeStructFromMap(T) removeImpl(::soup::joaat::hash(#T))
	};
}
