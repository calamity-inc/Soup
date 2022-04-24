#pragma once

#include "Writer.hpp"

namespace soup
{
	class StringWriter final : public Writer
	{
	public:
		std::string str{};

		StringWriter(bool little_endian = true)
			: Writer(little_endian)
		{
		}

		~StringWriter() final = default;

	protected:
		void write(const char* data, size_t size) final
		{
			str.append(data, size);
		}
	};
}
