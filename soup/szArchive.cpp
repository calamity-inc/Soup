#include "szArchive.hpp"

#include "BitReader.hpp"
#include "BitWriter.hpp"
#include "Reader.hpp"
#include "szFile.hpp"
#include "Writer.hpp"

namespace soup
{
	void szArchive::write(Writer& w)
	{
		/*uint8_t format_version = 0;
		w.u8(format_version);*/

		BitWriter bw(&w);
		for (const auto& file : files)
		{
			bw.str_utf8_nt(file.first);
			szFile::compress(bw, file.second);
		}
	}

	void szArchive::read(Reader& r)
	{
		/*uint8_t format_version;
		r.u8(format_version);*/

		BitReader br(&r);
		while (r.hasMore())
		{
			std::string name;
			br.str_utf8_nt(name);
			files.emplace(std::move(name), szFile::decompress(br));
		}
	}
}
