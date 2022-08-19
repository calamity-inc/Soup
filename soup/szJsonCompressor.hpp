#pragma once

#include "szCompressor.hpp"

#include "json.hpp"

namespace soup
{
	struct szJsonCompressor : public szCompressor
	{
		[[nodiscard]] szMethod getMethod() const noexcept final
		{
			return SM_JSON;
		}

		[[nodiscard]] bool isByteAligned() const noexcept final
		{
			return true;
		}

		void compress(BitWriter& bw, const std::string& data) const final
		{
			if (auto jt = json::decodeForDedicatedVariable(data))
			{
				jt->binaryEncode(bw.getStream());
			}
		}

		[[nodiscard]] virtual szPreservationLevel getPreservationLevel(const szCompressResult& res, const std::string& data) const
		{
			if (res.data.getBitLength() == 0)
			{
				return NONE;
			}
			if (checkDecompressed(res, data))
			{
				return LOSSLESS;
			}
			return LOSSY;
		}

		[[nodiscard]] std::string decompress(BitReader& br) const final
		{
			auto jt = json::binaryDecodeForDedicatedVariable(br.getStream());
			return jt->encodePretty();
		}
	};
}
