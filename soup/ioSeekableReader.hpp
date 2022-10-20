#pragma once

#include "Reader.hpp"

namespace soup
{
	class ioSeekableReader : public Reader
	{
	public:
		using Reader::Reader;

		[[nodiscard]] virtual size_t getPosition() = 0;

		virtual void seek(size_t pos) = 0;
		virtual void seekEnd() = 0;
	};
}
