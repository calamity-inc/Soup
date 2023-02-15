#pragma once

#include <string>

namespace soup
{
	struct WebSocket
	{
		[[nodiscard]] static std::string generateKey();
		[[nodiscard]] static std::string hashKey(std::string key);
		[[nodiscard]] static bool readFrame(bool& fin, uint8_t& opcode, std::string& data_in_payload_out);
	};
}
