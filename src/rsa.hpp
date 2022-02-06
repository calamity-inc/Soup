#pragma once

#include "bigint.hpp"

namespace soup
{
	struct rsa
	{
		struct key
		{
			bigint n;
			bigint e;

			[[nodiscard]] bigint pow_mod(const bigint& x) const;

			[[nodiscard]] bigint encrypt(const bigint& msg) const;
			[[nodiscard]] bigint decrypt(const bigint& msg) const;

			[[nodiscard]] bigint sign(const bigint& hash) const;
			[[nodiscard]] bool verify(const bigint& hash, const bigint& sig) const;
		};

		struct keypair
		{
			bigint n;
			bigint e;
			bigint d;

			[[nodiscard]] key getPublic() const;
			[[nodiscard]] key getPrivate() const;
		};
	};
}
