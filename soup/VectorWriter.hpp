#pragma once

#include "Writer.hpp"

namespace soup
{
	class VectorWriter final : public Writer
	{
	public:
		std::vector<char> vec{};

		VectorWriter(Endian endian = LITTLE_ENDIAN)
			: Writer(endian)
		{
		}

		VectorWriter(bool little_endian)
			: Writer(little_endian)
		{
		}

		~VectorWriter() final = default;

		void write(const char* data, size_t size) final
		{
			while (size--)
			{
				vec.emplace_back(*data++);
			}
		}
	};
}
