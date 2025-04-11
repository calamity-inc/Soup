#include "tunables.hpp"

#include <unordered_map>

#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	// No mutexing as we're assuming that tunables are only registered in single-threaded static init, and we only read the map afterwards.
	// Using a UniquePtr instead of unordered_map directly because static init order is not guaranteed and unordered_map would require init of its own.
	static UniquePtr<std::unordered_map<uint32_t, std::atomic<uint64_t>*>> s_tunables_map;

	std::atomic<uint64_t>* tunables::find(uint32_t name_joaat_hash) noexcept
	{
		if (s_tunables_map)
		{
			if (auto e = s_tunables_map->find(name_joaat_hash); e != s_tunables_map->end())
			{
				return e->second;
			}
		}
		return nullptr;
	}

	void tunables::internal_register(uint32_t name_joaat_hash, std::atomic<uint64_t>* pValue) noexcept
	{
		if (!s_tunables_map)
		{
			s_tunables_map = soup::make_unique<std::unordered_map<uint32_t, std::atomic<uint64_t>*>>();
		}
		s_tunables_map->emplace(name_joaat_hash, pValue);
	}
}
