#include "netIntel.hpp"

#include "bitutil.hpp"
#include "CidrSubnet4Interface.hpp"
#include "CidrSubnet6Interface.hpp"
#include "csv.hpp"
#include "deflate.hpp"
#include "FileWriter.hpp"
#include "Ipv6Maths.hpp"
#include "netIntelLocationData4OnDisk.hpp"
#include "string.hpp"
#include "StringReader.hpp"
#include "time.hpp"
#include "WebResource.hpp"

NAMESPACE_SOUP
{
	void netIntel::init(bool ipv4, bool ipv6)
	{
		asInit(ipv4, ipv6);
		locationInit(ipv4, ipv6);
	}

	void netIntel::deinit() noexcept
	{
		asDeinit();
		locationDeinit();
	}

	struct DynamicDataMeta
	{
		int64_t version;
		int64_t expiry;
		uint32_t as_pool_decompressed_size;
		uint32_t as_string_pool_decompressed_size;
		uint32_t ipv4_to_aso_decompressed_size;
		uint32_t ipv6_to_aso_decompressed_size;
	};

	void netIntel::asInit(bool ipv4, bool ipv6)
	{
		auto folder = filesystem::getProgramData();
		folder /= "Calamity, Inc";
		folder /= "Soup";
		folder /= "netIntel";
		std::filesystem::create_directories(folder);

		DynamicDataMeta meta;
		meta.version = 0;
		meta.expiry = 0;
		{
			size_t meta_size;
			if (auto meta_data = filesystem::createFileMapping(folder / "meta.bin", meta_size))
			{
				if (meta_size >= sizeof(DynamicDataMeta))
				{
					memcpy(&meta, meta_data, sizeof(DynamicDataMeta));
				}
				filesystem::destroyFileMapping(meta_data, meta_size);
			}
		}

		if (time::unixSeconds() > meta.expiry)
		{
			WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/meta.bin");
			if (wr.download(), wr.hasData())
			{
				if (wr.data.size() >= sizeof(DynamicDataMeta))
				{
					DynamicDataMeta remote_meta;
					memcpy(&remote_meta, wr.data.data(), sizeof(DynamicDataMeta));
					if (remote_meta.version != meta.version)
					{
						std::filesystem::remove(folder / "as_pool.bin");
						std::filesystem::remove(folder / "as_string_pool.bin");
						std::filesystem::remove(folder / "ipv4_to_aso.bin");
						std::filesystem::remove(folder / "ipv6_to_aso.bin");
						memcpy(&meta, &remote_meta, sizeof(DynamicDataMeta));
						std::ofstream of(folder / "meta.bin", std::ofstream::binary);
						of.write((const char*)&meta, sizeof(meta));
					}
				}
			}
		}

		if (meta.version)
		{
			if (!std::filesystem::exists(folder / "as_pool.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/as_pool.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "as_pool.bin", deflate::decompress(wr.data, meta.as_pool_decompressed_size).decompressed);
				}
			}

			if (!std::filesystem::exists(folder / "as_string_pool.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/as_string_pool.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "as_string_pool.bin", deflate::decompress(wr.data, meta.as_string_pool_decompressed_size).decompressed);
				}
			}

			if (ipv4 && !std::filesystem::exists(folder / "ipv4_to_aso.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/ipv4_to_aso.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "ipv4_to_aso.bin", deflate::decompress(wr.data, meta.ipv4_to_aso_decompressed_size).decompressed);
				}
			}

			if (ipv6 && !std::filesystem::exists(folder / "ipv6_to_aso.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/ipv6_to_aso.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "ipv6_to_aso.bin", deflate::decompress(wr.data, meta.ipv6_to_aso_decompressed_size).decompressed);
				}
			}
		}

		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "as_pool.bin", size))
			{
				as_map.init(data, size);
			}
		}

		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "as_string_pool.bin", size))
			{
				as_string_pool.init(data, size);
			}
		}

		if (ipv4)
		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "ipv4_to_aso.bin", size))
			{
				ipv4_to_aso.init(data, size);
			}
		}

		if (ipv6)
		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "ipv6_to_aso.bin", size))
			{
				ipv6_to_aso.init(data, size);
			}
		}

		initExtraWasm();
	}

	bool netIntel::asIsInited() noexcept
	{
		return as_map.begin != nullptr;
	}

	void netIntel::asDeinit() noexcept
	{
		as_map.reset();
		as_string_pool.reset();
		ipv4_to_aso.reset();
		ipv6_to_aso.reset();
	}

	void netIntel::locationInit(bool ipv4, bool ipv6)
	{
		if (ipv4)
		{
			initIpv4ToLocation();
		}
		if (ipv6)
		{
			initIpv6ToLocation();
		}
	}

	bool netIntel::locationIsInited() noexcept
	{
		return !location_pool.empty();
	}

	void netIntel::locationDeinit() noexcept
	{
		location_pool.clear();
		ipv4tolocation.clear();
		ipv6tolocation.clear();
	}

	void netIntel::initExtraWasm()
	{
		WebResource rsc("raw.githubusercontent.com", "/calamity-inc/soup-extra-data/senpai/build/release.wasm");
		rsc.downloadWithCaching();
		extra_wasm = std::move(rsc.data);
	}

	void netIntel::initIpv4ToLocation()
	{
		StringReader ipv4tolocationcsv;
		{
			WebResource rsc("raw.githubusercontent.com", "/sapics/ip-location-db/master/geolite2-city/geolite2-city-ipv4-num.csv.gz");
			rsc.downloadWithCaching();
			ipv4tolocationcsv = deflate::decompress(std::move(rsc.data)).decompressed;
		}
		ipv4tolocation.reserve(2'800'000);
		std::vector<std::string> arr;
		arr.reserve(6);
		for (std::string line; ipv4tolocationcsv.getLine(line); )
		{
			csv::parseLine(arr, line);
			SOUP_IF_UNLIKELY (arr.size() < 6)
			{
				continue;
			}
			ipv4tolocation.emplace(
				string::toIntOpt<uint32_t>(arr.at(0)).value(),
				string::toIntOpt<uint32_t>(arr.at(1)).value(),
				netIntelLocationData{
					std::move(arr.at(2)),
					location_pool.emplace(std::move(arr.at(3))),
					location_pool.emplace(std::move(arr.at(5))),
				}
			);
		}
	}

	void netIntel::initIpv6ToLocation()
	{
		StringReader ipv6tolocationcsv;
		{
			WebResource rsc("raw.githubusercontent.com", "/sapics/ip-location-db/master/geolite2-city/geolite2-city-ipv6.csv.gz");
			rsc.downloadWithCaching();
			ipv6tolocationcsv = deflate::decompress(std::move(rsc.data)).decompressed;
		}
		ipv6tolocation.reserve(700'000);
		std::vector<std::string> arr;
		arr.reserve(6);
		for (std::string line; ipv6tolocationcsv.getLine(line); )
		{
			csv::parseLine(arr, line);
			SOUP_IF_UNLIKELY (arr.size() < 6)
			{
				continue;
			}
			IpAddr begin, end;
			SOUP_ASSERT(begin.fromString(arr.at(0)));
			SOUP_ASSERT(end.fromString(arr.at(1)));
			ipv6tolocation.emplace(begin, end, netIntelLocationData{
				std::move(arr.at(2)),
				location_pool.emplace(std::move(arr.at(3))),
				location_pool.emplace(std::move(arr.at(5))),
			});
		}
	}

	Optional<netAs> netIntel::getAsByNumber(uint32_t number) const noexcept
	{
		if (auto data = as_map.find(number))
		{
			if (data->handle < as_string_pool.size && data->name < as_string_pool.size)
			{
				return netAs{ number, &as_string_pool.data[data->handle], &as_string_pool.data[data->name] };
			}
		}
		return {};
	}

	Optional<netAs> netIntel::getAsByIp(const IpAddr& addr) const
	{
		return addr.isV4()
			? getAsByIpv4(addr.getV4NativeEndian())
			: getAsByIpv6(addr)
			;
	}

	Optional<netAs> netIntel::getAsByIpv4(native_u32_t ip) const
	{
		if (auto aso = ipv4_to_aso.find(ip))
		{
			if (*aso < ((uintptr_t)as_map.end - (uintptr_t)as_map.begin))
			{
				auto entry = &as_map.begin[*aso / sizeof(decltype(as_map)::Entry)];
				if (entry->value.handle < as_string_pool.size && entry->value.name < as_string_pool.size)
				{
					return netAs{ entry->key, &as_string_pool.data[entry->value.handle], &as_string_pool.data[entry->value.name] };
				}
			}
		}
		return {};
	}

	Optional<netAs> netIntel::getAsByIpv6(const IpAddr& addr) const
	{
		if (auto aso = ipv6_to_aso.find(addr))
		{
			if (*aso < ((uintptr_t)as_map.end - (uintptr_t)as_map.begin))
			{
				auto entry = &as_map.begin[*aso / sizeof(decltype(as_map)::Entry)];
				if (entry->value.handle < as_string_pool.size && entry->value.name < as_string_pool.size)
				{
					return netAs{ entry->key, &as_string_pool.data[entry->value.handle], &as_string_pool.data[entry->value.name] };
				}
			}
		}
		return {};
	}

	std::vector<UniquePtr<CidrSubnetInterface>> netIntel::getRangesByAsn(uint32_t asn) const
	{
		std::vector<UniquePtr<CidrSubnetInterface>> res;

		uint32_t aso = -1;
		if (auto data = as_map.find(asn))
		{
			aso = (uint32_t)(((uintptr_t)data - 4) - (uintptr_t)as_map.begin);
		}

		for (auto i = ipv4_to_aso.begin; i != ipv4_to_aso.end; ++i)
		{
			if (i->data == aso)
			{
				UniquePtr<CidrSubnet4Interface> newElement = soup::make_unique<CidrSubnet4Interface>(
					IpAddr((native_u32_t)i->lower),
					static_cast<uint8_t>(31 - bitutil::getMostSignificantSetBit(/* i->upper - i->lower */ i->lower ^ i->upper))
				);

				auto it = res.begin();
				for (; it != res.end(); ++it)
				{
					if ((*it)->getAddr() == newElement->getAddr())
					{
						break;
					}
				}

				if (it != res.end())
				{
					if ((*it)->getSize() < newElement->getSize())
					{
						*it = std::move(newElement);
					}
				}
				else
				{
					res.emplace_back(std::move(newElement));
				}
			}
		}

		for (auto i = ipv6_to_aso.begin; i != ipv6_to_aso.end; ++i)
		{
			if (i->data == aso)
			{
				auto delta = Ipv6Maths::fromIpAddr(i->lower);
				Ipv6Maths::xorEq(delta, Ipv6Maths::fromIpAddr(i->upper));

				UniquePtr<CidrSubnet6Interface> newElement = soup::make_unique<CidrSubnet6Interface>(
					i->lower,
					(127 - Ipv6Maths::getMostSignificantSetBit(delta))
				);

				auto it = res.begin();
				for (; it != res.end(); ++it)
				{
					if ((*it)->getAddr() == newElement->getAddr())
					{
						break;
					}
				}

				if (it != res.end())
				{
					if ((*it)->getSize() < newElement->getSize())
					{
						*it = std::move(newElement);
					}
				}
				else
				{
					res.emplace_back(std::move(newElement));
				}
			}
		}

		return res;
	}

	const netIntelLocationData* netIntel::getLocationByIp(const IpAddr& addr) const
	{
		return addr.isV4()
			? getLocationByIpv4(addr.getV4NativeEndian())
			: getLocationByIpv6(addr)
			;
	}
	
	const netIntelLocationData* netIntel::getLocationByIpv4(native_u32_t ip) const
	{
		return ipv4tolocation.find(ip);
	}

	const netIntelLocationData* netIntel::getLocationByIpv6(const IpAddr& addr) const
	{
		return ipv6tolocation.find(addr);
	}

	void netIntel::locationExport(const std::filesystem::path& dir)
	{
		std::unordered_map<const char*, uint32_t> offsets{};
		offsets.reserve(location_pool.pool.size());
		{
			FileWriter fw(dir / "location_pool.bin");
			fw.throwIfFailed();
			for (const auto& loc : location_pool.pool)
			{
				offsets.emplace(loc.c_str(), static_cast<uint32_t>(fw.s.tellp()));
				fw.str_nt(loc);
			}
		}
		
		{
			FileWriter fw(dir / "ipv4tolocation.bin");
			fw.throwIfFailed();
			for (const auto& e : ipv4tolocation.data)
			{
				netIntelLocationData4OnDisk data;
				data.lower = e.lower;
				data.upper = e.upper;

				data.country_code = e.data.country_code.c_str();
				data.state_offset = offsets.at(e.data.state);
				data.city_offset = offsets.at(e.data.city);

				data.write(fw);
			}
		}
	}
}
