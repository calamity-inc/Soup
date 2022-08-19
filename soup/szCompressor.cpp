#include "szCompressor.hpp"

#include "BitReader.hpp"
#include "StringReader.hpp"
#include "szJsonCompressor.hpp"
#include "szStoreCompressor.hpp"

namespace soup
{
	UniquePtr<szCompressor> szCompressor::fromMethod(szMethod method)
	{
		switch (method)
		{
		case SM_STORE: return soup::make_unique<szStoreCompressor>();
		case SM_JSON: return soup::make_unique<szJsonCompressor>();

		case SM_SIZE: break;
		}
		return {};
	}

	bool szCompressor::isByteAligned() const noexcept
	{
		return false;
	}

	void szCompressor::setLevelOfPreservation(szCompressResult& res, const std::string& data) const
	{
		BitString bs(res.data);
		bs->finishByte();
		StringReader sr(std::move(bs.sw.str));
		BitReader br(&sr);
		res.level_of_preservation = ((decompress(br) == data) ? LOSSLESS : LOSSY);
	}
}
