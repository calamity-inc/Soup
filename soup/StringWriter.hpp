#pragma once

#include "Writer.hpp"

namespace soup
{
	class StringWriter final : public Writer
	{
	public:
		std::string str{};

		StringWriter(Endian endian = LITTLE_ENDIAN)
			: Writer(endian)
		{
		}

		StringWriter(bool little_endian)
			: Writer(little_endian)
		{
		}

		~StringWriter() final = default;

		void write(const char* data, size_t size) final
		{
			str.append(data, size);
		}
	};
}
