#pragma once

#include "szCompressor.hpp"

namespace soup
{
	struct szStoreCompressor : public szCompressor
	{
		[[nodiscard]] szMethod getMethod() const noexcept final
		{
			return SM_STORE;
		}

		[[nodiscard]] bool isByteAligned() const noexcept final
		{
			return true;
		}

		void compress(BitWriter& bw, const std::string& data) const final
		{
			bw.getStream().str_lp_u64_dyn(data);
		}

		[[nodiscard]] virtual szPreservationLevel getPreservationLevel(const szCompressResult& res, const std::string& data) const
		{
			return LOSSLESS;
		}

		[[nodiscard]] std::string decompress(BitReader& br) const final
		{
			std::string str;
			br.getStream().str_lp_u64_dyn(str);
			return str;
		}
	};
}
