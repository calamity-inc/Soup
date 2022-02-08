#pragma once

#include <cstdint>

namespace soup
{
#pragma pack(push, 1)
	template <size_t Bytes>
	struct bigbitset
	{
		uint8_t data[Bytes]{};

		[[nodiscard]] static bigbitset<Bytes>* at(void* dp) noexcept
		{
			return reinterpret_cast<bigbitset<Bytes>*>(dp);
		}

		[[nodiscard]] static const bigbitset<Bytes>* at(const void* dp) noexcept
		{
			return reinterpret_cast<const bigbitset<Bytes>*>(dp);
		}

		[[nodiscard]] bool get(const size_t i) const noexcept
		{
			const auto j = (i / 8);
			const auto k = (i % 8);

			return (data[j] >> k) & 1;
		}

		void set(const size_t i, const bool v) noexcept
		{
			const auto j = (i / 8);
			const auto k = (i % 8);

			const uint8_t mask = (1 << k);

			data[j] &= ~mask;
			data[j] |= (mask * v);
		}

		void enable(const size_t i) noexcept
		{
			const auto j = (i / 8);
			const auto k = (i % 8);

			data[j] |= (1 << k);
		}

		void disable(const size_t i) noexcept
		{
			const auto j = (i / 8);
			const auto k = (i % 8);

			data[j] &= ~(1 << k);
		}
	};
#pragma pack(pop)
}
