#pragma once

#include <string>
#include <vector>

namespace soup
{
	struct Oid
	{
		std::vector<uint32_t> path{};

		constexpr Oid() = default;

		Oid(std::initializer_list<uint32_t>&& path)
			: path(std::move(path))
		{
		}

		[[nodiscard]] static Oid fromBinary(const std::string& str);
		[[nodiscard]] static Oid fromBinary(std::istream& s);

		[[nodiscard]] bool operator ==(const Oid& b) const noexcept;
		[[nodiscard]] bool operator !=(const Oid& b) const noexcept;

		[[nodiscard]] std::string toString() const;

		friend std::ostream& operator<<(std::ostream& os, const Oid& v);
	};
}
