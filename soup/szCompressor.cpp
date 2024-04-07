#include "szCompressor.hpp"

#include "BitReader.hpp"
#include "StringReader.hpp"
#include "szEnglishCompressor.hpp"
#include "szJsonCompressor.hpp"
#include "szStoreCompressor.hpp"
#include "szUnicodeCompressor.hpp"

NAMESPACE_SOUP
{
	UniquePtr<szCompressor> szCompressor::fromMethod(szMethod method)
	{
		switch (method)
		{
		case SM_STORE: return soup::make_unique<szStoreCompressor>();
		case SM_JSON: return soup::make_unique<szJsonCompressor>();
		case SM_UNICODE: return soup::make_unique<szUnicodeCompressor>();
		case SM_ENGLISH: return soup::make_unique<szEnglishCompressor>();

		case SM_SIZE: break;
		}
		return {};
	}

	bool szCompressor::isByteAligned() const noexcept
	{
		return false;
	}

	szCompressResult szCompressor::compress(const std::string& data) const
	{
		szCompressResult res;
		compress(*res.data, data);
		res.preservation_level = getPreservationLevel(res, data);
		return res;
	}

	szPreservationLevel szCompressor::getPreservationLevel(const szCompressResult& res, const std::string& data) const
	{
		return checkDecompressed(res, data) ? LOSSLESS : CORRUPTED;
	}

	bool szCompressor::checkDecompressed(const szCompressResult& res, const std::string& data) const
	{
		BitString bs(res.data);
		bs->finishByte();
		StringReader sr(std::move(bs.sw.data));
		BitReader br(&sr);
		return decompress(br) == data;
	}
}
