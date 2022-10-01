#include "base.hpp"

#if SOUP_WASM || SOUP_CODE_INSPECTOR

#include "RaiiEmulator.hpp"

#include "base40.hpp"
#include "base64.hpp"
#include "Canvas.hpp"
#include "InquiryLang.hpp"
#include "InquiryObject.hpp"
#include "KeyGenId.hpp"
#include "Mixed.hpp"
#include "QrCode.hpp"
#include "rsa.hpp"

using namespace soup;

static RaiiEmulator heap{};
static std::string ret_str_buf{};

[[nodiscard]] static const char* ret_str(std::string&& str)
{
	ret_str_buf = std::move(str);
	return ret_str_buf.c_str();
}

#define returnString(x) return ret_str(x);

// [global namespace]

SOUP_CEXPORT void beginScope()
{
	heap.beginScope();
}

SOUP_CEXPORT void endScope()
{
	heap.endScope();
}

SOUP_CEXPORT void broadenScope(void* inst)
{
	heap.broadenScope(inst);
}

SOUP_CEXPORT void endLifetime(void* inst)
{
	heap.free(inst);
}

// base40

SOUP_CEXPORT const char* base40_encode(std::string* x)
{
	returnString(base40::encode(*x));
}

SOUP_CEXPORT std::string* base40_decode(const char* x)
{
	return heap.add(new std::string(base40::decode(x)));
}

// base64

SOUP_CEXPORT const char* base64_encode(std::string* x)
{
	returnString(base64::encode(*x));
}

// Bigint

SOUP_CEXPORT const char* Bigint_toString(Bigint* x)
{
	returnString(x->toString());
}

// Canvas

SOUP_CEXPORT unsigned int Canvas_getWidth(Canvas* x)
{
	return x->width;
}

SOUP_CEXPORT unsigned int Canvas_getHeight(Canvas* x)
{
	return x->height;
}

SOUP_CEXPORT void Canvas_resizeNearestNeighbour(Canvas* x, unsigned int desired_width, unsigned int desired_height)
{
	x->resizeNearestNeighbour(desired_width, desired_height);
}

SOUP_CEXPORT const char* Canvas_toSvg(Canvas* x, unsigned int scale)
{
	returnString(x->toSvg(scale));
}

SOUP_CEXPORT std::string* Canvas_toNewPngString(Canvas* x)
{
	return heap.add(x->toPng());
}

// InquiryLang

SOUP_CEXPORT Mixed* InquiryLang_execute(const char* x)
{
	if (auto res = InquiryLang::execute(x))
	{
		return res.release();
	}
	return nullptr;
}

SOUP_CEXPORT const char* InquiryLang_formatResultLine(Mixed* x)
{
	returnString(InquiryLang::formatResultLine(*x));
}

// InquiryObject

SOUP_CEXPORT bool InquiryObject_isCanvas(InquiryObject* x)
{
	return x->type == InquiryObject::CANVAS;
}

SOUP_CEXPORT Canvas* InquiryObject_getCanvas(InquiryObject* x)
{
	return &x->cap.get<Canvas>();
}

// KeyGenId

SOUP_CEXPORT KeyGenId* KeyGenId_newFromBinary(std::string* str)
{
	return heap.add(new KeyGenId(*str));
}

SOUP_CEXPORT KeyGenId* KeyGenId_generate()
{
	return heap.add(KeyGenId::generate());
}

SOUP_CEXPORT std::string* KeyGenId_toBinary(KeyGenId* x)
{
	return heap.add(x->toBinary());
}

SOUP_CEXPORT RsaKeypair* KeyGenId_getKeypair(KeyGenId* x, unsigned int bits)
{
	return heap.add(x->getKeypair(bits));
}

// Mixed

SOUP_CEXPORT bool Mixed_isInquiryObject(Mixed* x)
{
	return x->isInquiryObject();
}

SOUP_CEXPORT InquiryObject* Mixed_getInquiryObject(Mixed* x)
{
	return &x->getInquiryObject();
}

// QrCode

SOUP_CEXPORT QrCode* QrCode_newFromText(const char* x)
{
	return heap.add(QrCode::encodeText(x));
}

SOUP_CEXPORT Canvas* QrCode_toNewCanvas(QrCode* x, unsigned int border, bool black_bg)
{
	return heap.add(x->toCanvas(border, black_bg));
}

// RsaKeypair

SOUP_CEXPORT const Bigint* RsaKeypair_getN(const RsaKeypair* x)
{
	return &x->n;
}

SOUP_CEXPORT const Bigint* RsaKeypair_getP(const RsaKeypair* x)
{
	return &x->p;
}

SOUP_CEXPORT const Bigint* RsaKeypair_getQ(const RsaKeypair* x)
{
	return &x->q;
}

// std::exception

SOUP_CEXPORT const char* exception_what(std::exception* x)
{
	returnString(x->what());
}

#endif
