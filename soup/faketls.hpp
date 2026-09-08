#pragma once

#include "CertStore.hpp"
#include "SharedPtr.hpp"
#if !SOUP_WASM || SOUP_EMSCRIPTEN

NAMESPACE_SOUP
{
	// http://faketls.com
	struct faketls
	{
		static SharedPtr<CertStore> getLatestCert(); // blocking. return is default-initialised on request failure.
	};
}
#endif
