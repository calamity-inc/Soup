#pragma once

#include <string>

namespace soup
{
	struct WebSocket
	{
		[[nodiscard]] static std::string generateKey();
		[[nodiscard]] static std::string hashKey(std::string key);
		[[nodiscard]] static bool readFrame(std::string& data, bool& fin, uint8_t& opcode, std::string& payload);
	};
}
