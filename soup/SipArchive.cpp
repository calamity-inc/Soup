#include "SipArchive.hpp"

#include "BitReader.hpp"
#include "BitWriter.hpp"
#include "Reader.hpp"
#include "SipFile.hpp"
#include "Writer.hpp"

namespace soup
{
	void SipArchive::write(Writer& w)
	{
		/*uint8_t format_version = 0;
		w.u8(format_version);*/

		BitWriter bw(&w);
		for (const auto& file : files)
		{
			bw.str_utf8dyn(file.first);
			SipFile::compress(bw, file.second);
		}
	}

	void SipArchive::read(Reader& r)
	{
		/*uint8_t format_version;
		r.u8(format_version);*/

		BitReader br(&r);
		while (r.hasMore())
		{
			std::string name;
			br.str_utf8dyn(name);
			files.emplace(std::move(name), SipFile::decompress(br));
		}
	}
}
