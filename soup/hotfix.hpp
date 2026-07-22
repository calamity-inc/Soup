#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

NAMESPACE_SOUP
{
	struct hotfix
	{
		static std::unordered_map<uint32_t, uint32_t> snapshotU32Tunables();

		template <typename T>
		static std::unordered_map<uint32_t, T> computeChangedTunables(const std::unordered_map<uint32_t, T>& old_tunables, const std::unordered_map<uint32_t, T>& new_tunables)
		{
			std::unordered_map<uint32_t, T> map;
			for (const auto& old_e : old_tunables)
			{
				if (auto new_e = new_tunables.find(old_e.first); new_e != new_tunables.end())
				{
					if (old_e.second != new_e->second)
					{
						map.emplace(old_e.first, new_e->second);
					}
				}
			}
			return map;
		}

		static std::string packU32Tunables(const std::unordered_map<uint32_t, uint32_t>& map);

		static std::string packHotfix(const std::string& u32_pack);

		static void applyHotfix(cadInterface& cai);
	};
}
