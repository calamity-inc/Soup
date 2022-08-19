#pragma once

#include "szCompressor.hpp"

namespace soup
{
	struct szUnicodeCompresor : public szCompressor
	{
		[[nodiscard]] szMethod getMethod() const noexcept final
		{
			return SM_UNICODE;
		}

		void compress(BitWriter& bw, const std::string& data) const final
		{
			bw.str_utf8_nt(data);
		}

		[[nodiscard]] virtual szPreservationLevel getPreservationLevel(const szCompressResult& res, const std::string& data) const
		{
			return checkDecompressed(res, data) ? LOSSLESS : CORRUPTED;
		}

		[[nodiscard]] std::string decompress(BitReader& br) const final
		{
			std::string str{};
			br.str_utf8_nt(str);
			return str;
		}
	};
}
