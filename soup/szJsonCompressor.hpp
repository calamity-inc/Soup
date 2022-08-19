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

		[[nodiscard]] szCompressResult compress(const std::string& data) const final
		{
			szCompressResult res;
			if (auto jt = json::decodeForDedicatedVariable(data))
			{
				jt->binaryEncode(res.data->getStream());
				setLevelOfPreservation(res, data);
			}
			return res;
		}

		[[nodiscard]] std::string decompress(const BitReader& br) const final
		{
			auto jt = json::binaryDecodeForDedicatedVariable(br.getStream());
			return jt->encodePretty();
		}
	};
}
