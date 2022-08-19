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
		auto compressor = szCompressor::fromMethod(SM_STORE);
		BitString best_result(std::move(compressor->compress(data).data));

		for (unsigned int method = SM_STORE + 1; method != SM_SIZE; ++method)
		{
			compressor = szCompressor::fromMethod((szMethod)method);
			szCompressResult res = compressor->compress(data);
			if (res.level_of_preservation != NONE
				&& best_result.getBitLength() > res.data.getBitLength()
				)
			{
				best_method = method;
				best_result = std::move(res.data);
			}
		}

		bw.t(method_bits, best_method);
		if (compressor->isByteAligned())
		{
			bw.finishByte();
		}
		best_result.commit(bw);
	}

	std::string szFile::decompress(BitReader& br)
	{
		unsigned int method;
		SOUP_IF_UNLIKELY(!br.t<unsigned int>(method_bits, method))
		{
			return {};
		}
		auto compressor = szCompressor::fromMethod((szMethod)method);
		if (compressor->isByteAligned())
		{
			br.finishByte();
		}
		return compressor->decompress(br);
	}
}
