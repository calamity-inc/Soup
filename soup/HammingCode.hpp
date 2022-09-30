#pragma once

#include <utility>
#include <vector>

namespace soup
{
	// An implementation of extended hamming codes.
	struct HammingCode
	{
		enum Status
		{
			OK, // No errors -- or an even number of errors >2
			FIXED, // 1 error that has been fixed -- or an odd number of errors >1
			BAD, // exactly 2 errors
		};

		[[nodiscard]] static std::vector<bool> encode(const std::vector<bool>& in);
		[[nodiscard]] static std::pair<Status, std::vector<bool>> decode(std::vector<bool> data); // Note: Decoded vector will have additional zeroes at the end if packed size is not a power of 2.

		[[nodiscard]] static size_t getPackedSize(size_t required_data_bits);
		[[nodiscard]] static std::vector<bool> pack(const std::vector<bool>& data);
		[[nodiscard]] static size_t getUnpackedSize(size_t total_bits);
		[[nodiscard]] static std::vector<bool> unpack(const std::vector<bool>& data);

		[[nodiscard]] static size_t getErrorPosition(const std::vector<bool>& data);
		static void setAllParityBits(std::vector<bool>& data);
		static void setParityBits(std::vector<bool>& data);
		[[nodiscard]] static bool getExtendedParity(const std::vector<bool>& data);
		static Status checkAndFix(std::vector<bool>& data);
	};
}
