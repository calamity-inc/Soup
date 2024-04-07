#pragma once

#include <cstdint>

#include "base.hpp"

NAMESPACE_SOUP
{
	enum netMeshMsgType : uint8_t
	{
		MESH_MSG_LINK = 0,
		MESH_MSG_AFFIRMATIVE,
		MESH_MSG_NEGATIVE,
		MESH_MSG_AUTH_REQUEST,
		MESH_MSG_AUTH_CHALLENGE,
		MESH_MSG_AUTH_FINISH,
		MESH_MSG_CAPABILITIES,
		MESH_MSG_APPLICATION,
	};
}
