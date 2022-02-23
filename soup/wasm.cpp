#include "base.hpp"

#if SOUP_WASM || defined(__INTELLISENSE__)

#include <thread>

#include "asn1_sequence.hpp"
#include "base64.hpp"
#include "bigint.hpp"
#include "pem.hpp"
#include "rsa.hpp"

using namespace soup;

static std::string ret_str_buf{};

[[nodiscard]] static const char* ret_str(std::string&& str)
{
	ret_str_buf = std::move(str);
	return ret_str_buf.c_str();
}

#define returnString(x) return ret_str(std::move(x));

// asn1_sequence

SOUP_CEXPORT asn1_sequence* asn1_sequence_new(std::string* str)
{
	auto ret = new asn1_sequence(std::move(*str));
	delete str;
	return ret;
}

SOUP_CEXPORT void asn1_sequence_free(asn1_sequence* seq)
{
	delete seq;
}

SOUP_CEXPORT std::string* asn1_sequence_toDer(asn1_sequence* seq)
{
	return new std::string(seq->toDer());
}

SOUP_CEXPORT const char* asn1_sequence_toString(asn1_sequence* seq)
{
	returnString(seq->toString());
}

// base64

SOUP_CEXPORT const char* base64_encode(std::string* bin)
{
	std::string enc = base64::encode(*bin);
	delete bin;
	returnString(enc);
}

SOUP_CEXPORT std::string* base64_decode(const char* x)
{
	return new std::string(base64::decode(x));
}

// bigint

SOUP_CEXPORT bigint* bigint_newFromString(const char* str)
{
	return new bigint(bigint::fromString(str, strlen(str)));
}

SOUP_CEXPORT bigint* bigint_random(const size_t bits)
{
	return new bigint(bigint::random(bits));
}

SOUP_CEXPORT bigint* bigint_randomProbablePrime(const size_t bits)
{
	return new bigint(bigint::randomProbablePrime(bits));
}

SOUP_CEXPORT bigint* bigint_newCopy(void* b)
{
	return new bigint(*reinterpret_cast<bigint*>(b));
}

SOUP_CEXPORT void bigint_free(bigint* x)
{
	delete x;
}

SOUP_CEXPORT bigint* bigint_plus(bigint* a, bigint* b)
{
	return new bigint(*a + *b);
}

SOUP_CEXPORT void bigint_plusEq(bigint* a, bigint* b)
{
	*a += *b;
}

SOUP_CEXPORT const char* bigint_toString(bigint* x)
{
	returnString(x->toString());
}

// pem

SOUP_CEXPORT std::string* pem_decode(const char* x)
{
	return new std::string(pem::decode(x));
}

// rsa::keypair

SOUP_CEXPORT rsa::keypair* rsa_keypair_new(bigint* p, bigint* q)
{
	auto res = new rsa::keypair(std::move(*p), std::move(*q));
	delete p;
	delete q;
	return res;
}

SOUP_CEXPORT void rsa_keypair_free(rsa::keypair* kp)
{
	delete kp;
}

SOUP_CEXPORT rsa::key_private* rsa_keypair_getPrivate(rsa::keypair* kp)
{
	return new rsa::key_private(kp->getPrivate());
}

// rsa::key_private

SOUP_CEXPORT void rsa_key_private_free(rsa::key_private* key)
{
	delete key;
}

SOUP_CEXPORT asn1_sequence* rsa_key_private_toAsn1(rsa::key_private* key)
{
	return new asn1_sequence(key->toAsn1());
}

SOUP_CEXPORT const char* rsa_key_private_toPem(rsa::key_private* key)
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
