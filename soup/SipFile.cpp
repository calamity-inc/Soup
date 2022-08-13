#include "SipFile.hpp"

#include "BitReader.hpp"
#include "BitWriter.hpp"
#include "json.hpp"
#include "Reader.hpp"
#include "Writer.hpp"

namespace soup
{
	void SipFile::write(Writer& w)
	{
		uint8_t format_version = 0;
		w.u8(format_version);

		BitWriter bw(&w);
		for (const auto& file : files)
		{
			bw.str_utf8dyn(file.first);
			if (file.first.substr(file.first.length() - 5) == ".json")
			{
				auto jt = json::decodeForDedicatedVariable(file.second);
				bw.b(jt);
				bw.finishByte();
				if (jt)
				{
					jt->binaryEncode(w);
					continue;
				}
			}
			else
			{
				bw.finishByte();
			}
			w.str_lp_u64_dyn(file.second);
		}
	}

	void SipFile::read(Reader& r)
	{
		uint8_t format_version;
		r.u8(format_version);

		BitReader br(&r);
		while (r.hasMore())
		{
			std::string name;
			br.str_utf8dyn(name);
			if (name.substr(name.length() - 5) == ".json")
			{
				bool use_specialisation;
				br.b(use_specialisation);
				br.finishByte();
				if (use_specialisation)
				{
					auto jt = json::binaryDecodeForDedicatedVariable(r);
					files.emplace(std::move(name), jt->encodePretty());
					continue;
				}
			}
			else
			{
				br.finishByte();
			}
			std::string contents;
			r.str_lp_u64_dyn(contents);
			files.emplace(std::move(name), std::move(contents));
		}
	}
}
