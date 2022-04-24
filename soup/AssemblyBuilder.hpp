#pragma once

#include <cstdint>
#include <vector>

namespace soup
{
	class AssemblyBuilder
	{
	private:
		std::vector<uint8_t> m_data;

		template <size_t S>
		void addBytes(const uint8_t(&bytes)[S])
		{
			m_data.reserve(S);
			for (const auto& byte : bytes)
			{
				m_data.emplace_back(byte);
			}
		}

	public:
		void funcBegin();
		void funcEnd();
		void retn();
		void nop();

		void setA(uint64_t val);
		void setD(uint64_t val);
		void set8(uint64_t val);
		void set14(uint64_t val);

		void incRAX();

		void movPtrRAX(uint8_t val);

		void callA();

		void jmpA();

		uint8_t* data();
		size_t size() const noexcept;
	};
}
