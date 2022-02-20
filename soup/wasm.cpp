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

SOUP_CEXPORT void* asn1_sequence_new(void* str)
{
	auto ret = new asn1_sequence(std::move(*reinterpret_cast<std::string*>(str)));
	delete reinterpret_cast<std::string*>(str);
	return ret;
}

SOUP_CEXPORT void asn1_sequence_free(void* x)
{
	delete reinterpret_cast<asn1_sequence*>(x);
}

SOUP_CEXPORT const char* asn1_sequence_toString(void* x)
{
	returnString(reinterpret_cast<asn1_sequence*>(x)->toString());
}

// base64

SOUP_CEXPORT const char* base64_encode(const char* x)
{
	returnString(base64::encode(x));
}

SOUP_CEXPORT void* base64_decode(const char* x)
{
	return new std::string(base64::decode(x));
}

// bigint

SOUP_CEXPORT void* bigint_newFromString(const char* str)
{
	return new bigint(bigint::fromString(str, strlen(str)));
}

SOUP_CEXPORT void* bigint_newCopy(void* b)
{
	return new bigint(*reinterpret_cast<bigint*>(b));
}

SOUP_CEXPORT void bigint_free(void* x)
{
	delete reinterpret_cast<bigint*>(x);
}

SOUP_CEXPORT void* bigint_plus(void* a, void* b)
{
	return new bigint(*reinterpret_cast<bigint*>(a) + *reinterpret_cast<bigint*>(b));
}

SOUP_CEXPORT void bigint_plusEq(void* a, void* b)
{
	*reinterpret_cast<bigint*>(a) += *reinterpret_cast<bigint*>(b);
}

SOUP_CEXPORT const char* bigint_toString(void* x)
{
	returnString(reinterpret_cast<bigint*>(x)->toString());
}

// pem

SOUP_CEXPORT void* pem_decode(const char* x)
{
	return new std::string(pem::decode(x));
}

// string

SOUP_CEXPORT const char* string_val(void* x)
{
	return reinterpret_cast<std::string*>(x)->c_str();
}

SOUP_CEXPORT void string_free(void* x)
{
	delete reinterpret_cast<std::string*>(x);
}

#endif
