#pragma once

#include "szCompressor.hpp"

NAMESPACE_SOUP
{
	struct szUnicodeCompressor : public szCompressor
	{
		[[nodiscard]] szMethod getMethod() const noexcept final
		{
			return SM_UNICODE;
		}

		void compress(BitWriter& bw, const std::string& data) const final
		{
			bw.str_utf8_nt(data);
		}

		[[nodiscard]] std::string decompress(BitReader& br) const final
		{
			std::string str{};
			br.str_utf8_nt(str);
			return str;
		}
	};
}
