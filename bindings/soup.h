// Fun Fact: #pragma once is supported by literally every compiler despite not being part of any standard
#ifndef _SOUP_H
#define _SOUP_H

#if !defined(SOUP_CEXPORT) || SOUP_CODE_INSPECTOR
	#ifdef _WIN32
		#define SOUP_EXPORT __declspec(dllexport)
	#else
		#define SOUP_EXPORT
	#endif

	#define SOUP_CEXPORT extern "C" SOUP_EXPORT

	typedef void Bigint;
	typedef void Canvas;
	typedef void InquiryObject;
	typedef void KeyGenId;
	typedef void Mixed;
	typedef void QrCode;
	typedef void RsaKeypair;

	typedef void stdexception;
	typedef void stdstring;
#endif

// [global namespace]
SOUP_CEXPORT void beginScope();
SOUP_CEXPORT void endScope();
SOUP_CEXPORT void broadenScope(void* inst);
SOUP_CEXPORT void endLifetime(void* inst);
// base64
SOUP_CEXPORT const char* base40_encode(stdstring* x);
SOUP_CEXPORT stdstring* base40_decode(const char* x);
// base64
SOUP_CEXPORT const char* base64_encode(stdstring* x);
// Bigint
SOUP_CEXPORT const char* Bigint_toString(Bigint* x);
// Canvas
SOUP_CEXPORT unsigned int Canvas_getWidth(Canvas* x);
SOUP_CEXPORT unsigned int Canvas_getHeight(Canvas* x);
SOUP_CEXPORT void Canvas_resizeNearestNeighbour(Canvas* x, unsigned int desired_width, unsigned int desired_height);
SOUP_CEXPORT const char* Canvas_toSvg(Canvas* x, unsigned int scale);
SOUP_CEXPORT stdstring* Canvas_toNewPngString(Canvas* x);
// InquiryLang
SOUP_CEXPORT Mixed* InquiryLang_execute(const char* x);
SOUP_CEXPORT const char* InquiryLang_formatResultLine(Mixed* x);
// InquiryObject
SOUP_CEXPORT bool InquiryObject_isCanvas(InquiryObject* x);
SOUP_CEXPORT Canvas* InquiryObject_getCanvas(InquiryObject* x);
// KeyGenId
SOUP_CEXPORT KeyGenId* KeyGenId_newFromSeedsExport(unsigned int bits, stdstring* str);
SOUP_CEXPORT KeyGenId* KeyGenId_generate(unsigned int bits);
SOUP_CEXPORT stdstring* KeyGenId_toSeedsExport(KeyGenId* x);
SOUP_CEXPORT RsaKeypair* KeyGenId_getKeypair(KeyGenId* x);
// Mixed
SOUP_CEXPORT bool Mixed_isInquiryObject(Mixed* x);
SOUP_CEXPORT InquiryObject* Mixed_getInquiryObject(Mixed* x);
// QrCode
SOUP_CEXPORT QrCode* QrCode_newFromText(const char* x);
SOUP_CEXPORT Canvas* QrCode_toNewCanvas(QrCode* x, unsigned int border, bool black_bg);
// RsaKeypair
SOUP_CEXPORT const Bigint* RsaKeypair_getN(const RsaKeypair* x);
SOUP_CEXPORT const Bigint* RsaKeypair_getP(const RsaKeypair* x);
SOUP_CEXPORT const Bigint* RsaKeypair_getQ(const RsaKeypair* x);
// exception
SOUP_CEXPORT const char* exception_what(stdexception* x);

#endif // #ifndef _SOUP_H
