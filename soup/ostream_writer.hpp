#pragma once

#include "writer.hpp"

namespace soup
{
	class ostream_writer final : public writer
	{
	public:
		std::ostream* os;

		ostream_writer(std::ostream* os, bool little_endian = true)
			: writer(little_endian), os(os)
		{
		}

		~ostream_writer() final = default;

	protected:
		void write(const char* data, size_t size) final
		{
			os->write(data, size);
		}
	};
}
