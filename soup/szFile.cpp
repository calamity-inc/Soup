#include "szFile.hpp"

#include "BitReader.hpp"
#include "bitutil.hpp"
#include "BitWriter.hpp"
#include "json.hpp"
#include "szMethod.hpp"

namespace soup
{
	static constexpr auto method_bits = bitutil::getBitsNeededToEncodeRange(SM_SIZE);

	void szFile::compress(BitWriter& bw, const std::string& data)
	{
		auto jt = json::decodeForDedicatedVariable(data);
		bw.t<unsigned int>(method_bits, jt ? SM_JSON : SM_PLAIN);
		if (jt)
		{
			bw.finishByte();
			jt->binaryEncode(bw.getStream());
		}
		else
		{
			bw.finishByte();
			bw.getStream().str_lp_u64_dyn(data);
		}
	}

	std::string szFile::decompress(BitReader& br)
	{
		unsigned int method;
		br.t<unsigned int>(method_bits, method);
		std::string out;
		switch (method)
		{
		case SM_PLAIN:
			br.finishByte();
			br.getStream().str_lp_u64_dyn(out);
			break;

		case SM_JSON:
			{
				br.finishByte();
				auto jt = json::binaryDecodeForDedicatedVariable(br.getStream());
				out = jt->encodePretty();
			}
			break;

		case SM_SIZE:
			break;
		}
		return out;
	}
}
