#pragma once

#include <cstdint>

namespace soup
{
	// chess
	struct ChessCoordinate;

	// crypto
	struct Keystore;

	// crypto.x509
	struct Certchain;

	// data
	struct Oid;

	// data.asn1
	struct Asn1Sequence;

	// data.json
	struct JsonArray;
	struct JsonBool;
	struct JsonFloat;
	struct JsonInt;
	struct JsonObject;
	struct JsonString;

	// math
	class Bigint;
	struct Vector3;

	// math.3d
	struct BoxCorners;
	class Matrix;
	struct Mesh;
	struct Poly;
	struct Ray;

	// mem
	struct AllocRaiiRemote;
	struct Pattern;
	struct CompiletimePatternWithOptBytesBase;
	class Pointer;
	class Range;
	struct VirtualRegion;

	// net
	class IpAddr;
	class Server;
	struct ServerService;
	class Socket;

	// net.tls
	class SocketTlsHandshaker;
	struct TlsServerRsaData;
	struct TlsClientHello;

	// net.web
	class HttpRequest;

	// net.web.websocket
	struct WebSocketMessage;

	// os
	enum ControlInput : uint8_t;
	class Module;
	enum MouseButton : uint8_t;

	// os.windows
	struct HandleRaii;

	// task
	class PromiseBase;
	class Scheduler;

	// vis
	class Canvas;
	struct RasterFont;

	// vis.ui.conui
	struct ConuiApp;
	struct ConuiDiv;

	// vis.ui.editor
	struct Editor;
	struct EditorText;
}
