#include "Pattern.hpp"

#include "Module.hpp"
#include "CompiletimePatternWithOptBytesBase.hpp"
#include "Pointer.hpp"
#include "VirtualRegion.hpp"

namespace soup
{
	Pattern::Pattern(const CompiletimePatternWithOptBytesBase& sig)
		: bytes(sig.getVec())
	{
		for (auto& byte : bytes)
		{
			if (byte.has_value())
			{
				byte = byte.value() ^ 0x0F;
			}
		}
	}

	Pattern::Pattern(std::string_view ida_sig)
	{
		addBytesFromIdaSig(ida_sig);
	}

	void Pattern::addBytesFromIdaSig(std::string_view ida_sig)
	{
		auto to_upper = [](char c) -> char
		{
			return c >= 'a' && c <= 'z' ? static_cast<char>(c + ('A' - 'a')) : static_cast<char>(c);
		};

		auto to_hex = [&](char c) -> std::optional<std::uint8_t>
		{
			switch (to_upper(c))
			{
			case '0':
				return static_cast<std::uint8_t>(0);
			case '1':
				return static_cast<std::uint8_t>(1);
			case '2':
				return static_cast<std::uint8_t>(2);
			case '3':
				return static_cast<std::uint8_t>(3);
			case '4':
				return static_cast<std::uint8_t>(4);
			case '5':
				return static_cast<std::uint8_t>(5);
			case '6':
				return static_cast<std::uint8_t>(6);
			case '7':
				return static_cast<std::uint8_t>(7);
			case '8':
				return static_cast<std::uint8_t>(8);
			case '9':
				return static_cast<std::uint8_t>(9);
			case 'A':
				return static_cast<std::uint8_t>(10);
			case 'B':
				return static_cast<std::uint8_t>(11);
			case 'C':
				return static_cast<std::uint8_t>(12);
			case 'D':
				return static_cast<std::uint8_t>(13);
			case 'E':
				return static_cast<std::uint8_t>(14);
			case 'F':
				return static_cast<std::uint8_t>(15);
			default:
				return std::nullopt;
			}
		};

		for (std::size_t i = 0; i < ida_sig.size(); ++i)
		{
			if (ida_sig[i] == ' ')
				continue;

			bool last = (i == ida_sig.size() - 1);
			if (ida_sig[i] != '?')
			{
				if (!last)
				{
					auto c1 = to_hex(ida_sig[i]);
					auto c2 = to_hex(ida_sig[i + 1]);

					if (c1 && c2)
					{
						bytes.emplace_back(static_cast<std::uint8_t>((*c1 * 0x10) + *c2));
					}
				}
			}
			else
			{
				bytes.emplace_back(std::nullopt);
			}
		}
	}

#if SOUP_WINDOWS
	VirtualRegion Pattern::virtual_scan(BYTE* startAddress)
	{
		auto compareMemory = [](const std::uint8_t* data, std::optional<std::uint8_t>* elem, std::size_t num) -> bool
		{
			for (std::size_t i = 0; i < num; ++i)
			{
				if (elem[i].has_value() && data[i] != elem[i].value())
				{
					return false;
				}
			}
			return true;
		};

		MEMORY_BASIC_INFORMATION mbi{};
		auto&& baseOffset = startAddress;

		while (VirtualQuery(baseOffset, &mbi, sizeof mbi) > 0)
		{
			//try
			{
				if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE && mbi.Type == MEM_PRIVATE && mbi.RegionSize > 0x80000)
				{
					auto start = baseOffset;
					auto end = start + (mbi.RegionSize - bytes.size());

					for (auto&& i = start; i < end; ++i)
					{
						if (compareMemory(i, bytes.data(), bytes.size()))
						{
							//g_logger.log(fmt::format("{} in region with {}", (void*)i, mbi.RegionSize));
							return VirtualRegion(baseOffset, mbi.RegionSize, i);
						}
					}
				}
			}
			//catch (...)
			//{
			//}
			baseOffset += mbi.RegionSize;
			mbi = {};
		}

		return VirtualRegion(nullptr, 0, nullptr);
	}

	std::vector<VirtualRegion> Pattern::virtual_scan_all(unsigned int limit)
	{
		std::vector<VirtualRegion> result{};
		BYTE* addr = nullptr;
		while (true)
		{
			auto region = virtual_scan(addr);
			if (region.pointer == nullptr)
			{
				break;
			}
			addr = region.pointer + 8;
			result.emplace_back(std::move(region));
			if (result.size() >= limit)
			{
				break;
			}
		}
		return result;
	}
#endif
}
