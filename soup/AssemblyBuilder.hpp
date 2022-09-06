#pragma once

#include <cstdint>
#include <vector>

#include "fwd.hpp"

namespace soup
{
	struct AssemblyBuilder
	{
		std::vector<uint8_t> m_data;

		template <size_t S>
		void addBytes(const uint8_t(&bytes)[S])
		{
			for (const auto& byte : bytes)
			{
				m_data.emplace_back(byte);
			}
		}

		void funcBegin();
		void funcEnd();
		void retn();
		void nop();

		void setA(uint64_t val);
		void setD(uint64_t val);
		void set8(uint64_t val);
		void set14(uint64_t val);

		void setAtoC(); // mov rax, rcx
		void set12toC(); // mov r12, rcx
		void setCto12(); // mov rcx, r12

		void incRAX();

		void movPtrRAX(uint8_t val);

		void callA();

		void jmpA();

		uint8_t* data();
		const uint8_t* data() const;
		size_t size() const noexcept;

		[[nodiscard]] UniquePtr<AllocRaiiLocalBase> allocate() const;
	};
}
