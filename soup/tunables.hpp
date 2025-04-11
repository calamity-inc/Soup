#pragma once

#include <atomic>

#include "joaat.hpp"

// Example usage:
// static SOUP_TUNABLE(uint32_t, MY_TUNABLE) = 69;
// soup::tunables::set("MY_TUNABLE", 420);

#define SOUP_TUNABLE(T, name) ::soup::Tunable<T, ::soup::joaat::compileTimeHash(#name)> name

NAMESPACE_SOUP
{
	struct tunables
	{
		[[nodiscard]] static std::atomic<uint64_t>* find(const char* name) noexcept
		{
			return find(soup::joaat::hash(name));
		}

		[[nodiscard]] static std::atomic<uint64_t>* find(uint32_t name_joaat_hash) noexcept;

		static void set(const char* name, uint64_t value) noexcept
		{
			if (auto pValue = find(name))
			{
				pValue->store(value);
			}
		}

		static void set(uint32_t name_joaat_hash, uint64_t value) noexcept
		{
			if (auto pValue = find(name_joaat_hash))
			{
				pValue->store(value);
			}
		}

		static void internal_register(uint32_t name_joaat_hash, std::atomic<uint64_t>* pValue) noexcept;
	};
	

	template <typename T, uint32_t name_joaat_hash>
	struct Tunable
	{
		std::atomic<uint64_t> value;

		Tunable() noexcept
		{
			tunables::internal_register(name_joaat_hash, &value);
		}

		Tunable(T default_value) noexcept
			: value(static_cast<uint64_t>(default_value))
		{
			tunables::internal_register(name_joaat_hash, &value);
		}

		void operator=(T value) noexcept
		{
			this->value.store(static_cast<uint64_t>(value));
		}

		[[nodiscard]] operator T() const noexcept
		{
			return static_cast<T>(value.load());
		}
	};
}
