#pragma once

#include <string>
#include <vector>

namespace soup
{
	struct oid
	{
		std::vector<uint32_t> path{};

		constexpr oid() = default;

		oid(std::initializer_list<uint32_t>&& path)
			: path(std::move(path))
		{
		}

		[[nodiscard]] static oid fromBinary(const std::string& str);
		[[nodiscard]] static oid fromBinary(std::istream& s);

		[[nodiscard]] bool operator ==(const oid& b) const noexcept;
		[[nodiscard]] bool operator !=(const oid& b) const noexcept;

		[[nodiscard]] std::string toString() const;

		friend std::ostream& operator<<(std::ostream& os, const oid& v);
	};
}
