#pragma once

#include <string>
#include <vector>

namespace soup
{
	struct Oid
	{
		std::vector<uint32_t> path{};

		static Oid COMMON_NAME;
		static Oid RSA_ENCRYPTION;
		static Oid SHA256_WITH_RSA_ENCRYPTION;

		constexpr Oid() = default;

		Oid(std::initializer_list<uint32_t>&& path)
			: path(std::move(path))
		{
		}

		[[nodiscard]] static Oid fromBinary(const std::string& str);
		[[nodiscard]] static Oid fromBinary(std::istream& s);

		[[nodiscard]] bool operator ==(const Oid& b) const noexcept;
		[[nodiscard]] bool operator !=(const Oid& b) const noexcept;

		[[nodiscard]] std::string toDer() const;
		[[nodiscard]] std::string toString() const;

		friend std::ostream& operator<<(std::ostream& os, const Oid& v);
	};
}
