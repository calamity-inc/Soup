#pragma once

#include "Writer.hpp"

namespace soup
{
	class StringPtrWriter final : public Writer
	{
	public:
		std::string* str;

		StringPtrWriter(std::string* str, bool little_endian = true)
			: Writer(little_endian), str(str)
		{
		}

		~StringPtrWriter() final = default;

		void write(const char* data, size_t size) final
		{
			str->append(data, size);
		}
	};
}
