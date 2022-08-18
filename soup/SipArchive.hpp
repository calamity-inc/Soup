#pragma once

#include <string>
#include <unordered_map>

#include "fwd.hpp"

namespace soup
{
	/*
	* Sip is a prototype of what a zip alternative that is truly focused on minimising the file size would look like.
	*
	* The only way I think we can really make a leap here is by specialising for every file type, which does mean going somewhat lossy.
	* For example, when we have a .json file, we don't particularly care about the spacing and indentation; we care about the data.
	* And if the data is all we have to care about, we can get worthwhile results:
	* - Input: 486 bytes
	* - .zip: 403 bytes (DEFLATE)
	* - .7z: 385 bytes (LZMA2:12)
	* - .sip: 263 bytes (JSON specialisation)
	*/
	struct SipArchive
	{
		std::unordered_map<std::string, std::string> files{};

		void write(Writer& w);
		void read(Reader& r);
	};
}
