#pragma once

#include <cstdint>

namespace soup
{
	enum netMeshMsgType : uint8_t
	{
		MESH_MSG_LINK = 0,
		MESH_MSG_OK,
		MESH_MSG_DNS_ADD_RECORD,
	};
}
