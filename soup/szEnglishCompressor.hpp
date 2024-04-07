#pragma once

#include "szCompressor.hpp"

NAMESPACE_SOUP
{
	struct szEnglishCompressor : public szCompressor
	{
		[[nodiscard]] szMethod getMethod() const noexcept final
		{
			return SM_ENGLISH;
		}

		[[nodiscard]] static const std::vector<std::string>& getWordList();

		void compress(BitWriter& bw, const std::string& data) const final;
		[[nodiscard]] std::string decompress(BitReader& br) const final;
	};
}
