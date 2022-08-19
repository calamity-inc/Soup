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

		[[nodiscard]] szCompressResult compress(const std::string& data) const final
		{
			szCompressResult res;
			res.data->getStream().str_lp_u64_dyn(data);
			res.level_of_preservation = LOSSLESS;
			return res;
		}

		[[nodiscard]] std::string decompress(const BitReader& br) const final
		{
			std::string str;
			br.getStream().str_lp_u64_dyn(str);
			return str;
		}
	};
}
