#pragma once

#include <string>

namespace soup
{
	struct WebSocket
	{
		enum ReadFrameStatus : uint8_t
		{
			OK = 0,
			PAYLOAD_INCOMPLETE,
			BAD,
		};

		[[nodiscard]] static std::string generateKey();
		[[nodiscard]] static std::string hashKey(std::string key);
		[[nodiscard]] static ReadFrameStatus readFrame(std::string& data, bool& fin, uint8_t& opcode, std::string& payload);
	};
}
