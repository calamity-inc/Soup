#include "base.hpp"

#if SOUP_WASM || defined(__INTELLISENSE__)

#include "asn1_sequence.hpp"
#include "base64.hpp"
#include "bigint.hpp"
#include "pem.hpp"

using namespace soup;

static std::string ret_str_buf{};

[[nodiscard]] static const char* ret_str(std::string&& str)
{
	ret_str_buf = std::move(str);
	return ret_str_buf.c_str();
}

#define returnString(x) return ret_str(x);

// asn1_sequence

SOUP_CEXPORT asn1_sequence* asn1_sequence_new(std::string* str)
{
	auto ret = new asn1_sequence(std::move(*str));
	delete str;
	return ret;
}

SOUP_CEXPORT void asn1_sequence_free(asn1_sequence* x)
{
	delete x;
}

SOUP_CEXPORT const char* asn1_sequence_toString(asn1_sequence* x)
{
	returnString(x->toString());
}

// base64

SOUP_CEXPORT const char* base64_encode(const char* x)
{
	returnString(base64::encode(x));
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
