#include "HammingCode.hpp"

#include "bitutil.hpp"

namespace soup
{
	std::vector<bool> HammingCode::encode(const std::vector<bool>& in)
	{
		auto out = pack(in);
		setAllParityBits(out);
		return out;
	}

	std::pair<HammingCode::Status, std::vector<bool>> HammingCode::decode(std::vector<bool> data)
	{
		auto status = checkAndFix(data);
		return { status, unpack(data) };
	}

	size_t HammingCode::getPackedSize(size_t required_data_bits)
	{
		for (uint8_t p = 2; p != SOUP_BITS; ++p)
		{
			uint8_t reserved_bits = (p + 1);
			size_t chunk_total_bits = ((size_t)1 << p);
			size_t chunk_data_bits = (chunk_total_bits - reserved_bits);
			if (chunk_data_bits >= required_data_bits)
			{
				return chunk_total_bits;
			}
		}
		SOUP_ASSERT_UNREACHABLE;
	}

	[[nodiscard]] static bool ispowerof2(size_t i) noexcept
	{
		return ((i & (i - 1)) == 0);
	}

	std::vector<bool> HammingCode::pack(const std::vector<bool>& data)
	{
		std::vector<bool> out(getPackedSize(data.size()), false);

		size_t data_i = 0;
		size_t out_i = 3;
		for (; data_i != data.size(); ++out_i)
		{
			SOUP_IF_LIKELY (!ispowerof2(out_i))
			{
				out.at(out_i) = data.at(data_i++);
			}
		}

		return out;
	}

	size_t HammingCode::getUnpackedSize(size_t total_bits)
	{
		const auto p = bitutil::getBitsNeededToEncodeRange(total_bits);
		const uint8_t reserved_bits = (p + 1);
		return (total_bits - reserved_bits);
	}

	std::vector<bool> HammingCode::unpack(const std::vector<bool>& data)
	{
		std::vector<bool> out(getUnpackedSize(data.size()), false);

		size_t data_i = 3;
		size_t out_i = 0;
		for (; data_i != data.size(); ++data_i)
		{
			SOUP_IF_LIKELY (!ispowerof2(data_i))
			{
				out.at(out_i++) = data.at(data_i);
			}
		}

		return out;
	}

	size_t HammingCode::getErrorPosition(const std::vector<bool>& data)
	{
		size_t res = 0;
		for (size_t i = 0; i != data.size(); ++i)
		{
			if (data.at(i))
			{
				res ^= i;
			}
		}
		return res;
	}

	void HammingCode::setAllParityBits(std::vector<bool>& data)
	{
		setParityBits(data);
		data.at(0) = getExtendedParity(data);
	}

	void HammingCode::setParityBits(std::vector<bool>& data)
	{
		size_t bs = getErrorPosition(data);
		const auto p = bitutil::getBitsNeededToEncodeRange(data.size());
		for (size_t i = 0; i != p; ++i)
		{
			data.at(1 << i) = ((bs >> i) & 1);
		}
	}

	bool HammingCode::getExtendedParity(const std::vector<bool>& data)
	{
		bool res = false;
		for (size_t i = 1; i != data.size(); ++i)
		{
			res ^= data.at(i);
		}
		return res;
	}

	HammingCode::Status HammingCode::checkAndFix(std::vector<bool>& data)
	{
		Status status = OK;

		auto errpos = getErrorPosition(data);
		if (errpos != 0
			|| getExtendedParity(data) != data.at(0)
			)
		{
			data.at(errpos).flip();
			status = FIXED;
		}

		return getExtendedParity(data) == data.at(0)
			? status
			: BAD
			;
	}
}
