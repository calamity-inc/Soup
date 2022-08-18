#pragma once

#include <string>
#include <unordered_map>

#include "fwd.hpp"

namespace soup
{
	/*
	* Sip is a prototype of what a zip alternative that is truly focused on minimising the file size would look like.
	* A big part of this is making specialisations for common formats to beat the more generic DEFLATE and LZMA2 algorithms.
	* 
	* A SipArchive is a collection of multiple files. The recommended file extension is ".sip", although no compatibility is guaranteed at this stage.
	*/
	struct SipArchive
	{
		std::unordered_map<std::string, std::string> files{};

		void write(Writer& w);
		void read(Reader& r);
	};
}
