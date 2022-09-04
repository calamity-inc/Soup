#pragma once

#include <cstdint>

namespace soup
{
	// algos.rand
	struct algRng;

	// chess
	struct ChessCoordinate;

	// crypto
	struct Keystore;
	struct RsaKeypair;

	// crypto.x509
	struct X509Certchain;

	// data
	struct Oid;
	template <typename T> class UniquePtr;

	// data.asn1
	struct Asn1Sequence;

	// data.json
	struct JsonArray;
	struct JsonBool;
	struct JsonFloat;
	struct JsonInt;
	struct JsonObject;
	struct JsonString;

	// io.bits
	class BitReader;
	class BitWriter;

	// io.stream
	class Reader;
	class StringReader;
	class Writer;

	// lang
	struct astBlock;
	struct astNode;
	class LangDesc;
	struct Lexeme;
	class ParserState;
	struct Token;

	// math
	class Bigint;
	struct Vector2;
	struct Vector3;

	// math.3d
	struct BoxCorners;
	class Matrix;
	struct Mesh;
	struct Poly;
	class Quaternion;
	struct Ray;

	// math.3d.scene
	struct Scene;

	// mem
	struct Pattern;
	struct CompiletimePatternWithOptBytesBase;
	class Pointer;
	class Range;
	struct VirtualRegion;
	
	// mem.alloc
	struct AllocRaiiLocalBase;
	struct AllocRaiiRemote;

	// net
	class IpAddr;
	class Server;
	struct ServerService;
	class Socket;
	struct SocketAddr;

	// net.dns.resolver
	struct dnsResolver;

	// net.tls
	class SocketTlsHandshaker;
	struct TlsServerRsaData;
	struct TlsClientHello;

	// net.web
	class HttpRequest;
	struct HttpResponse;

	// net.web.websocket
	struct WebSocketMessage;

	// os
	enum ControlInput : uint8_t;
	class Module;
	enum MouseButton : uint8_t;
	struct Window;

	// os.windows
	struct HandleRaii;

	// task
	class PromiseBase;
	class Scheduler;

	// util
	class Mixed;
	struct Status;

	// vis
	class Canvas;
	struct FormattedText;
	struct RasterFont;
	struct Rgb;

	// vis.layout
	struct lyoContainer;
	struct lyoDocument;
	struct lyoElement;
	struct lyoFlatDocument;
	struct lyoTextElement;

	// vis.render
	struct RenderTarget;

	// vis.ui.conui
	struct ConuiApp;
	struct ConuiDiv;

	// vis.ui.editor
	struct Editor;
	struct EditorText;
}
