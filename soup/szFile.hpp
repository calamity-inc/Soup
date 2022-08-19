#pragma once

#include "fwd.hpp"

#include <string>

namespace soup
{
	/*
	* Sip/sz is a prototype of what a zip alternative that is truly focused on minimising the file size would look like.
	* A big part of this is making specialisations for common formats to beat the more generic DEFLATE and LZMA2 algorithms.
	*
	* A SipFile is a single, compressed file. The recommended file extension is ".sz", although no compatibility is guaranteed at this stage.
	*/
	struct szFile
	{
		static void compress(BitWriter& bw, const std::string& data);
		[[nodiscard]] static std::string decompress(BitReader& br);
	};
}
