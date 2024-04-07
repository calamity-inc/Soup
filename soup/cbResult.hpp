#pragma once

#include <cstdint>

#include "base.hpp"
#include "Mixed.hpp"

NAMESPACE_SOUP
{
	enum cbResultType : uint8_t
	{
		CB_RES_BUILTIN = 0,
		CB_RES_CAPABILITIES, // "What can you do?"
		CB_RES_DELETE,
		CB_RES_IMAGE,
	};

	struct cbResult
	{
		cbResultType type;
		std::string response;
		Mixed extra;

		cbResult(const char* response)
			: type(CB_RES_BUILTIN), response(response)
		{
		}

		cbResult(std::string&& response)
			: type(CB_RES_BUILTIN), response(std::move(response))
		{
		}

		cbResult(cbResultType type)
			: type(type), response("This command is not implemented. :/")
		{
		}

		cbResult(cbResultType type, std::string&& response)
			: type(type), response(std::move(response))
		{
		}

		[[nodiscard]] bool isDelete() const noexcept
		{
			return type == CB_RES_DELETE;
		}

		[[nodiscard]] int64_t getDeleteNum() const
		{
			return extra.getInt();
		}
	};
}
