#pragma once

#include <cstdint>

namespace soup
{
	enum cbResultType : uint8_t
	{
		CB_RES_BUILTIN = 0,
		CB_RES_CAPABILITIES, // "What can you do?"
		CB_RES_DELETE,
	};

	struct cbResult
	{
		struct DeleteArgs
		{
			int num;
		};

		cbResultType type;
		std::string response;
		union
		{
			DeleteArgs delete_args;
		};

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

		[[nodiscard]] const DeleteArgs& getDeleteArgs() const
		{
			SOUP_IF_UNLIKELY (!isDelete())
			{
				throw 0;
			}
			return delete_args;
		}

		[[nodiscard]] int getDeleteNum() const
		{
			return getDeleteArgs().num;
		}
	};
}
