#pragma once

#include "laFrontend.hpp"

#include <cstdint>
#include <string>
#include <vector>

#include "UniquePtr.hpp"

namespace soup
{
	class laMathFrontend : public laFrontend
	{
	public:
		[[nodiscard]] irModule parse(const std::string& program) final;

	protected:
		struct Token
		{
			enum Type : uint8_t
			{
				T_VAL,
				T_ADD,
				T_SUB,
				T_MUL,
				T_DIV,
				T_MOD,
				T_END,
			};

			Type type;
			int64_t value;

			[[nodiscard]] uint8_t getBinaryOperator() const noexcept;
		};

		[[nodiscard]] static std::vector<Token> tokenise(const std::string& code);
		[[nodiscard]] static UniquePtr<irExpression> expr(const Token*& tk, uint8_t limit = 0);
	};
}
