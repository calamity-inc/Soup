#include "WebResource.hpp"

#if SOUP_WASM
#include <emscripten/fetch.h>
#else
#include "HttpRequest.hpp"
#endif

namespace soup
{
	WebResource::WebResource(std::string&& host, std::string&& path)
		: host(std::move(host)), path(std::move(path))
	{
	}

#if SOUP_WASM
	static WebResource* fetch_wr;
	static Callback<void(WebResource&)> fetch_cb;

	static void fetchSuccess(emscripten_fetch_t* fetch)
	{
		fetch_wr->has_data = true;
		fetch_wr->data = std::string(fetch->data, fetch->numBytes);
		emscripten_fetch_close(fetch);
		if (fetch_cb)
		{
			fetch_cb(*fetch_wr);
		}
	}

	static void fetchFail(emscripten_fetch_t* fetch)
	{
		emscripten_fetch_close(fetch);
	}

	void WebResource::download(Callback<void(WebResource&)>&& cb)
	{
		std::string url{};
		url.append("https://");
		url.append(host);
		url.append(path);

		fetch_wr = this;
		fetch_cb = std::move(cb);

		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);
		strcpy(attr.requestMethod, "GET");
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
		attr.onsuccess = &fetchSuccess;
		attr.onerror = &fetchFail;
		emscripten_fetch(&attr, url.c_str());
	}
#else
	void WebResource::download(Callback<void(WebResource&)>&& cb)
	{
		download();
		cb(*this);
	}

	void WebResource::download()
	{
		HttpRequest req{ std::string(host), std::string(path) };
		auto res = req.execute();
		if (res.has_value())
		{
			has_data = true;
			data = std::move(res->body);
		}
	}
#endif
}
