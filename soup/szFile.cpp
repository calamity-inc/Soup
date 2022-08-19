#include "szFile.hpp"

#include "BitReader.hpp"
#include "bitutil.hpp"
#include "szCompressor.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	static constexpr auto method_bits = bitutil::getBitsNeededToEncodeRange(SM_SIZE);

	void szFile::compress(BitWriter& bw, const std::string& data)
	{
		unsigned int best_method = SM_STORE;
		size_t best_result = -1;

		for (unsigned int method = 0; method != SM_SIZE; ++method)
		{
			auto compressor = szCompressor::fromMethod((szMethod)method);
			szCompressResult res = compressor->compress(data);
			//std::cout << "Method " << method << ": " << (res.data.getBitLength() / 8) << " bytes, preservation level " << (int)res.preservation_level << "\n";
			if (res.preservation_level > CORRUPTED
				&& best_result > res.data.getBitLength()
				)
			{
				best_method = method;
				best_result = res.data.getBitLength();
			}
		}

		bw.t(method_bits, best_method);
		auto compressor = szCompressor::fromMethod((szMethod)best_method);
		if (compressor->isByteAligned())
		{
			bw.finishByte();
		}
		compressor->compress(bw, data);
	}

	std::string szFile::decompress(BitReader& br)
	{
		unsigned int method = 0;
		br.t(method_bits, method);
		auto compressor = szCompressor::fromMethod((szMethod)method);
		if (compressor->isByteAligned())
		{
			br.finishByte();
		}
		return compressor->decompress(br);
	}
}
