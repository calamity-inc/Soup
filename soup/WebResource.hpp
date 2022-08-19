#pragma once

#include "base.hpp"

#include "Callback.hpp"
#include <filesystem>
#include <string>

namespace soup
{
	struct WebResource
	{
		std::string host;
		std::string path;

		uint64_t data_expires = 0;
		std::string data{};

		WebResource(std::string&& host, std::string&& path);

		[[nodiscard]] bool hasData() const noexcept
		{
			return data_expires != 0;
		}

		[[nodiscard]] bool isDataExpired() const noexcept;

#if SOUP_WASM
		void download(Callback<void(WebResource&)>&& cb = {});
#else
		void download(Callback<void(WebResource&)>&& cb); // blocking
		void download(); // blocking
#endif

		std::filesystem::path getCacheFile() const;

		bool store();
		bool restore();

		void downloadWithCaching(); // blocking
		void downloadWithStaleCaching(); // blocking
	};
}
