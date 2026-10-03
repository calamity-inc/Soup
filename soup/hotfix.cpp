#include "hotfix.hpp"

#include <algorithm>
#include <vector>

#include "MemoryRefReader.hpp"
#include "StringWriter.hpp"
#include "tunables.hpp"
#include "utility.hpp" // SOUP_MOVE_RETURN

NAMESPACE_SOUP
{
	std::unordered_map<uint32_t, uint32_t> hotfix::snapshotU32Tunables()
	{
		std::unordered_map<uint32_t, uint32_t> map;
		for (const auto& e : tunables<uint32_t>::internal_get_map())
		{
			map.emplace(e.first, *e.second);
		}
		return map;
	}

	std::string hotfix::packU32Tunables(const std::unordered_map<uint32_t,uint32_t>& map)
	{
		std::vector<std::pair<uint32_t, uint32_t>> vec;
		for (const auto& e : map)
		{
			vec.emplace_back(e.first, e.second);
		}
		std::sort(vec.begin(), vec.end(), [](const std::pair<uint32_t, uint32_t>& a, const std::pair<uint32_t, uint32_t>& b)
		{
			return a.first < b.first;
		});

		StringWriter sw;
		for (auto& e : vec)
		{
			sw.u32_le(e.first);
			sw.u32_le(e.second);
		}
		SOUP_MOVE_RETURN(sw.data);
	}

	void hotfix::applyU32Tunables(const std::string& u32_pack)
	{
		MemoryRefReader r(u32_pack);
		while (r.hasMore())
		{
			uint32_t hash, value;
			r.u32_le(hash);
			r.u32_le(value);
			tunables<uint32_t>::set(hash, value);
		}
	}
}
