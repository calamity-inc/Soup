#include "hotfix.hpp"

#include <algorithm>
#include <vector>

#include "cadArchive.hpp"
#include "cadInterface.hpp"
#include "md5.hpp"
#include "Packet.hpp"
#include "StringWriter.hpp"
#include "tunables.hpp"
#include "utility.hpp"

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

	std::string hotfix::packHotfix(const std::string& u32_pack)
	{
		const std::string u32_pack_hash = md5::hash(u32_pack);

		// Write root object
		StringWriter sw;
		{ uint8_t version = 0; sw.u8(version); }
		sw.str(md5::DIGEST_BYTES, u32_pack_hash);

		const std::string& root = sw.data;
		const std::string root_hash = md5::hash(root);

		cadArchive car;
		car.hash_bytes = md5::DIGEST_BYTES;
		car.hash_function = "md5";
		car.root_hash = root_hash;
		car.objects.emplace(root_hash, root);
		car.objects.emplace(u32_pack_hash, u32_pack);

		return car.toBinaryString();
	}

	void hotfix::applyHotfix(cadInterface& cai)
	{
		const auto root_hash = cai.getRootHash();
		SOUP_ASSERT(!root_hash.empty());

		const auto root = cai.getContent(root_hash);
		std::string u32_pack_hash;
		{
			MemoryRefReader r(root);
			uint8_t version;
			r.u8(version);
			SOUP_ASSERT(version == 0);
			r.str(md5::DIGEST_BYTES, u32_pack_hash);
		}

		if (const auto u32_pack = cai.getContent(u32_pack_hash); !u32_pack.empty())
		{
			MemoryRefReader r(u32_pack);
			do
			{
				uint32_t hash, value;
				r.u32_le(hash);
				r.u32_le(value);
				tunables<uint32_t>::set(hash, value);
			} while (r.hasMore());
		}
	}
}
