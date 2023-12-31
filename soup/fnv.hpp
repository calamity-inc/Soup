#pragma once

#include "Bigint.hpp"

namespace soup
{
	using namespace ::soup::literals;

	[[nodiscard]] inline uint32_t fnv1a_32(const std::string& str) noexcept
	{
		uint32_t hash = 2166136261u;
		for (const auto& c : str)
		{
			hash ^= c;
			hash *= 16777619u;
		}
		return hash;
	}

	[[nodiscard]] inline uint64_t fnv1a_64(const std::string& str) noexcept
	{
		uint64_t hash = 14695981039346656037ull;
		for (const auto& c : str)
		{
			hash ^= c;
			hash *= 1099511628211ull;
		}
		return hash;
	}

	[[nodiscard]] inline Bigint fnv1a_128(const std::string& str)
	{
		Bigint hash = "144066263297769815596495629667062367629"_b;
		const Bigint m = Bigint::_2pow(128);
		for (const auto& c : str)
		{
			hash.setChunkInbounds(0, hash.getChunkInbounds(0) ^ c);
			hash *= "309485009821345068724781371"_b;
			hash %= m;
		}
		return hash/*.toBinary(128 / 8)*/;
	}

	[[nodiscard]] inline Bigint fnv1a_256(const std::string& str)
	{
		Bigint hash = "100029257958052580907070968620625704837092796014241193945225284501741471925557"_b;
		const Bigint m = Bigint::_2pow(256);
		for (const auto& c : str)
		{
			hash.setChunkInbounds(0, hash.getChunkInbounds(0) ^ c);
			hash *= "374144419156711147060143317175368453031918731002211"_b;
			hash %= m;
		}
		return hash/*.toBinary(256 / 8)*/;
	}

	[[nodiscard]] inline Bigint fnv1a_512(const std::string& str)
	{
		Bigint hash = "9659303129496669498009435400716310466090418745672637896108374329434462657994582932197716438449813051892206539805784495328239340083876191928701583869517785"_b;
		const Bigint m = Bigint::_2pow(512);
		for (const auto& c : str)
		{
			hash.setChunkInbounds(0, hash.getChunkInbounds(0) ^ c);
			hash *= "35835915874844867368919076489095108449946327955754392558399825615420669938882575126094039892345713852759"_b;
			hash %= m;
		}
		return hash/*.toBinary(512 / 8)*/;
	}

	[[nodiscard]] inline Bigint fnv1a_1024(const std::string& str)
	{
		Bigint hash = "14197795064947621068722070641403218320880622795441933960878474914617582723252296732303717722150864096521202355549365628174669108571814760471015076148029755969804077320157692458563003215304957150157403644460363550505412711285966361610267868082893823963790439336411086884584107735010676915"_b;
		const Bigint m = Bigint::_2pow(1024);
		for (const auto& c : str)
		{
			hash.setChunkInbounds(0, hash.getChunkInbounds(0) ^ c);
			hash *= "5016456510113118655434598811035278955030765345404790744303017523831112055108147451509157692220295382716162651878526895249385292291816524375083746691371804094271873160484737966720260389217684476157468082573"_b;
			hash %= m;
		}
		return hash/*.toBinary(1024 / 8)*/;
	}
}
