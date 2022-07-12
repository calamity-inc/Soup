#include "base.hpp"

#if SOUP_WASM || SOUP_CODE_INSPECTOR

#include "Asn1Sequence.hpp"
#include "base64.hpp"
#include "Bigint.hpp"
#include "Canvas.hpp"
#include "pem.hpp"
#include "QrCode.hpp"
#include "rsa.hpp"

using namespace soup;

static std::string ret_str_buf{};

[[nodiscard]] static const char* ret_str(std::string&& str)
{
	ret_str_buf = std::move(str);
	return ret_str_buf.c_str();
}

#define returnString(x) return ret_str(x);

// asn1_sequence

SOUP_CEXPORT Asn1Sequence* asn1_sequence_new(std::string* str)
{
	auto ret = new Asn1Sequence(std::move(*str));
	delete str;
	return ret;
}

SOUP_CEXPORT void asn1_sequence_free(Asn1Sequence* seq)
{
	delete seq;
}

SOUP_CEXPORT std::string* asn1_sequence_toDer(Asn1Sequence* seq)
{
	return new std::string(seq->toDer());
}

SOUP_CEXPORT const char* asn1_sequence_toString(Asn1Sequence* seq)
{
	returnString(seq->toString());
}

// base64

SOUP_CEXPORT const char* base64_encode(std::string* bin)
{
	std::string enc = base64::encode(*bin);
	delete bin;
	returnString(std::move(enc));
}

SOUP_CEXPORT std::string* base64_decode(const char* x)
{
	return new std::string(base64::decode(x));
}

// bigint

SOUP_CEXPORT Bigint* bigint_newFromString(const char* str)
{
	return new Bigint(Bigint::fromString(str, strlen(str)));
}

SOUP_CEXPORT Bigint* bigint_random(const size_t bits)
{
	return new Bigint(Bigint::random(bits));
}

SOUP_CEXPORT Bigint* bigint_randomProbablePrime(const size_t bits)
{
	return new Bigint(Bigint::randomProbablePrime(bits));
}

SOUP_CEXPORT Bigint* bigint_newCopy(void* b)
{
	return new Bigint(*reinterpret_cast<Bigint*>(b));
}

SOUP_CEXPORT void bigint_free(Bigint* x)
{
	delete x;
}

SOUP_CEXPORT Bigint* bigint_plus(Bigint* a, Bigint* b)
{
	return new Bigint(*a + *b);
}

SOUP_CEXPORT void bigint_plusEq(Bigint* a, Bigint* b)
{
	*a += *b;
}

SOUP_CEXPORT const char* bigint_toString(Bigint* x)
{
	returnString(x->toString());
}

// pem

SOUP_CEXPORT std::string* pem_decode(const char* x)
{
	return new std::string(pem::decode(x));
}

// qr_code

SOUP_CEXPORT QrCode* qr_code_encodeText(const char* x)
{
	return new QrCode(QrCode::encodeText(x));
}

SOUP_CEXPORT void qr_code_free(QrCode* x)
{
	delete x;
}

SOUP_CEXPORT const char* qr_code_toSvg(QrCode* x, unsigned int border, bool black_bg, size_t scale)
{
	returnString(x->toCanvas(border, black_bg).toSvg(scale));
}

// rsa::keypair

SOUP_CEXPORT rsa::Keypair* rsa_keypair_new(Bigint* p, Bigint* q)
{
	auto res = new rsa::Keypair(std::move(*p), std::move(*q));
	delete p;
	delete q;
	return res;
}

SOUP_CEXPORT void rsa_keypair_free(rsa::Keypair* kp)
{
	delete kp;
}

SOUP_CEXPORT rsa::PrivateKey* rsa_keypair_getPrivate(rsa::Keypair* kp)
{
	return new rsa::PrivateKey(kp->getPrivate());
}

// rsa::key_private

SOUP_CEXPORT void rsa_key_private_free(rsa::PrivateKey* key)
{
	delete key;
}

SOUP_CEXPORT Asn1Sequence* rsa_key_private_toAsn1(rsa::PrivateKey* key)
{
	return new Asn1Sequence(key->toAsn1());
}

SOUP_CEXPORT const char* rsa_key_private_toPem(rsa::PrivateKey* key)
{
	returnString(key->toPem());
}

// string

SOUP_CEXPORT const char* string_val(std::string* x)
{
	return x->c_str();
}

SOUP_CEXPORT void string_free(std::string* x)
{
	delete x;
}

#endif
