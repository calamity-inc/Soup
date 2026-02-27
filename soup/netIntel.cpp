#include "netIntel.hpp"
#if !SOUP_WASM || SOUP_EMSCRIPTEN

#include <fstream>

#include "bitutil.hpp"
#include "CidrSubnet4Interface.hpp"
#include "CidrSubnet6Interface.hpp"
#include "deflate.hpp"
#include "Ipv6Maths.hpp"
#include "string.hpp"
#include "time.hpp"
#include "WebResource.hpp"

NAMESPACE_SOUP
{
	struct DynamicDataMeta
	{
		int64_t version;
		int64_t expiry;
		uint32_t as_pool_decompressed_size;
		uint32_t as_string_pool_decompressed_size;
		uint32_t ipv4_to_aso_decompressed_size;
		uint32_t ipv6_to_aso_decompressed_size;
		uint32_t location_string_pool_decompressed_size;
		uint32_t ipv4_to_location_decompressed_size;
		uint32_t ipv6_to_location_decompressed_size;
		uint32_t padding;
		char location_md5[16];
	};

	void netIntel::initEx(bool as_ipv4, bool as_ipv6, bool loc_ipv4, bool loc_ipv6)
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
						if (memcmp(meta.location_md5, remote_meta.location_md5, sizeof(meta.location_md5)) != 0)
						{
							std::filesystem::remove(folder / "location_string_pool.bin");
							std::filesystem::remove(folder / "ipv4_to_location.bin");
							std::filesystem::remove(folder / "ipv6_to_location.bin");
						}
						memcpy(&meta, &remote_meta, sizeof(DynamicDataMeta));
						std::ofstream of(folder / "meta.bin", std::ofstream::binary);
						of.write((const char*)&meta, sizeof(meta));
					}
				}
			}
		}

		if (meta.version)
		{
			if ((as_ipv4 || as_ipv6) && !std::filesystem::exists(folder / "as_pool.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/as_pool.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "as_pool.bin", deflate::decompress(wr.data, meta.as_pool_decompressed_size).decompressed);
				}
			}

			if ((as_ipv4 || as_ipv6) && !std::filesystem::exists(folder / "as_string_pool.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/as_string_pool.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "as_string_pool.bin", deflate::decompress(wr.data, meta.as_string_pool_decompressed_size).decompressed);
				}
			}

			if (as_ipv4 && !std::filesystem::exists(folder / "ipv4_to_aso.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/ipv4_to_aso.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "ipv4_to_aso.bin", deflate::decompress(wr.data, meta.ipv4_to_aso_decompressed_size).decompressed);
				}
			}

			if (as_ipv6 && !std::filesystem::exists(folder / "ipv6_to_aso.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/ipv6_to_aso.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "ipv6_to_aso.bin", deflate::decompress(wr.data, meta.ipv6_to_aso_decompressed_size).decompressed);
				}
			}

			if ((loc_ipv4 || loc_ipv6) && !std::filesystem::exists(folder / "location_string_pool.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/location_string_pool.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "location_string_pool.bin", deflate::decompress(wr.data, meta.location_string_pool_decompressed_size).decompressed);
				}
			}

			if (loc_ipv4 && !std::filesystem::exists(folder / "ipv4_to_location.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/ipv4_to_location.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "ipv4_to_location.bin", deflate::decompress(wr.data, meta.ipv4_to_location_decompressed_size).decompressed);
				}
			}

			if (loc_ipv6 && !std::filesystem::exists(folder / "ipv6_to_location.bin"))
			{
				WebResource wr("raw.githubusercontent.com", "/calamity-inc/soup-dynamic-data/refs/heads/senpai/ipv6_to_location.bin.gz");
				if (wr.download(), wr.hasData())
				{
					string::toFile(folder / "ipv6_to_location.bin", deflate::decompress(wr.data, meta.ipv6_to_location_decompressed_size).decompressed);
				}
			}
		}

		if (as_ipv4 || as_ipv6)
		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "as_pool.bin", size))
			{
				as_map.init(data, size);
			}
		}

		if (as_ipv4 || as_ipv6)
		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "as_string_pool.bin", size))
			{
				as_string_pool.init(data, size);
			}
		}

		if (as_ipv4)
		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "ipv4_to_aso.bin", size))
			{
				ipv4_to_aso.init(data, size);
			}
		}

		if (as_ipv6)
		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "ipv6_to_aso.bin", size))
			{
				ipv6_to_aso.init(data, size);
			}
		}

		if (as_ipv4 || as_ipv6)
		{
			initExtraWasm();
		}

		if (loc_ipv4 || loc_ipv6)
		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "location_string_pool.bin", size))
			{
				location_string_pool.init(data, size);
			}
		}

		if (loc_ipv4)
		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "ipv4_to_location.bin", size))
			{
				ipv4_to_location.init(data, size);
			}
		}

		if (loc_ipv6)
		{
			size_t size;
			if (const void* data = filesystem::createFileMapping(folder / "ipv6_to_location.bin", size))
			{
				ipv6_to_location.init(data, size);
			}
		}
	}

	void netIntel::deinit() noexcept
	{
		asDeinit();
		locationDeinit();
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

	bool netIntel::locationIsInited() noexcept
	{
		return location_string_pool.data != nullptr;
	}

	void netIntel::locationDeinit() noexcept
	{
		location_string_pool.reset();
		ipv4_to_location.reset();
		ipv6_to_location.reset();
	}

	void netIntel::initExtraWasm()
	{
		WebResource rsc("raw.githubusercontent.com", "/calamity-inc/soup-extra-data/senpai/build/release.wasm");
		rsc.downloadWithCaching();
		extra_wasm = std::move(rsc.data);
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

	Optional<netIntelLocationData> netIntel::getLocationByIp(const IpAddr& addr) const
	{
		return addr.isV4()
			? getLocationByIpv4(addr.getV4NativeEndian())
			: getLocationByIpv6(addr)
			;
	}
	
	Optional<netIntelLocationData> netIntel::getLocationByIpv4(native_u32_t ip) const
	{
		if (auto data = ipv4_to_location.find(ip))
		{
			netIntelLocationData res{ data->country_code };
			if (data->state < location_string_pool.size && data->city < location_string_pool.size)
			{
				res.state = &location_string_pool.data[data->state];
				res.city = &location_string_pool.data[data->city];
			}
			return res;
		}
		return {};
	}

	Optional<netIntelLocationData> netIntel::getLocationByIpv6(const IpAddr& addr) const
	{
		if (auto data = ipv6_to_location.find(addr))
		{
			netIntelLocationData res{ data->country_code };
			if (data->state < location_string_pool.size && data->city < location_string_pool.size)
			{
				res.state = &location_string_pool.data[data->state];
				res.city = &location_string_pool.data[data->city];
			}
			return res;
		}
		return {};
	}
}

#endif
