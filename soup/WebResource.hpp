#pragma once

#include "base.hpp"

#include "Callback.hpp"
#include <string>

namespace soup
{
	struct WebResource
	{
		std::string host;
		std::string path;

		bool has_data = false;
		std::string data{};

		WebResource(std::string&& host, std::string&& path);

#if SOUP_WASM
		void download(Callback<void(WebResource&)>&& cb = {});
#else
		void download(Callback<void(WebResource&)>&& cb); // blocking
		void download(); // blocking
#endif
	};
}
