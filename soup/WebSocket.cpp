#include "WebSocket.hpp"

#include "base64.hpp"
#include "rand.hpp"
#include "sha1.hpp"
#include "StringRefReader.hpp"

namespace soup
{
	std::string WebSocket::generateKey()
	{
		uint8_t b[16];
		rand.fill(b);
		return base64::encode((const char*)b, sizeof(b));
	}

	std::string WebSocket::hashKey(std::string key)
	{
		key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
		return base64::encode(sha1::hash(key));
	}

	bool WebSocket::readFrame(std::string& data, bool& fin, uint8_t& opcode, std::string& payload)
	{
		StringRefReader r(data, false);
		uint8_t buf;
		size_t header_size = 2;

		SOUP_IF_UNLIKELY (!r.u8(buf))
		{
			return false;
		}
		fin = (buf >> 7);
		opcode = (buf & 0x7F);

		SOUP_IF_UNLIKELY (!r.u8(buf))
		{
			return false;
		}
		bool has_mask = (buf >> 7);
		uint64_t payload_len = (buf & 0x7F);

		if (payload_len == 126)
		{
			header_size += (16 / 8);
			uint16_t buf;
			SOUP_IF_UNLIKELY (!r.u16(buf))
			{
				return false;
			}
			payload_len = buf;
		}
		else if (payload_len == 127)
		{
			header_size += (64 / 8);
			SOUP_IF_UNLIKELY (!r.u64(payload_len))
			{
				return false;
			}
		}

		std::string mask;
		if (has_mask)
		{
			header_size += 4;
			SOUP_IF_UNLIKELY (!r.str(4, mask))
			{
				return false;
			}
		}
		payload = data.substr(header_size, payload_len);
		data.erase(0, header_size + payload_len);
		if (has_mask)
		{
			for (auto i = 0; i != payload.size(); ++i)
			{
				payload[i] ^= mask.at(i % 4);
			}
		}
		return true;
	}
}
