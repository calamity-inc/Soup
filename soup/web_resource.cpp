#include "web_resource.hpp"

#if SOUP_WASM
#include <emscripten/fetch.h>
#else
#include "http_request.hpp"
#endif

namespace soup
{
	web_resource::web_resource(std::string&& host, std::string&& path)
		: host(std::move(host)), path(std::move(path))
	{
	}

#if SOUP_WASM
	static web_resource* fetch_wr;
	static callback<void(web_resource&)> fetch_cb;

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

	void web_resource::download(callback<void(web_resource&)>&& cb)
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
	void web_resource::download(callback<void(web_resource&)>&& cb)
	{
		download();
		cb(*this);
	}

	void web_resource::download()
	{
		http_request req{ std::string(host), std::string(path) };
		req.execute(callback<void(http_response&&)>([](http_response&& res, const capture& cap)
		{
			web_resource& wr = *cap.get<web_resource*>();
			wr.has_data = true;
			wr.data = std::move(res.body);
		}, this), callback<void()>([]{}));
	}
#endif
}
