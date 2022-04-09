#pragma once

#include "writer.hpp"

namespace soup
{
	class string_writer final : public writer
	{
	public:
		std::string str{};

		string_writer(bool little_endian = true)
			: writer(little_endian)
		{
		}

		~string_writer() final = default;

	protected:
		void write(const char* data, size_t size) final
		{
			str.append(data, size);
		}
	};
}
