#include "WebResource.hpp"

#if SOUP_WASM
#include <emscripten/fetch.h>
#else
#include "FileReader.hpp"
#include "FileWriter.hpp"
#include "filesystem.hpp"
#include "HttpRequest.hpp"
#include "sha1.hpp"
#include "string.hpp"
#endif
#include "time.hpp"

NAMESPACE_SOUP
{
	WebResource::WebResource(std::string&& host, std::string&& path)
		: host(std::move(host)), path(std::move(path))
	{
	}

	bool WebResource::isDataExpired() const noexcept
	{
		return static_cast<uint64_t>(time::unixSeconds()) > data_expires;
	}

#if SOUP_WASM
	static WebResource* fetch_wr;
	static Callback<void(WebResource&)> fetch_cb;

	static void fetchSuccess(emscripten_fetch_t* fetch)
	{
		fetch_wr->data_expires = time::unixSeconds() + (60 * 60 * 24);
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
		for (uint8_t i = 0; i != 3; ++i)
		{
			auto res = req.execute();
			if (!res.has_value())
			{
				break;
			}
			auto e = res->header_fields.find("Location");
			if (e == res->header_fields.end())
			{
				// TODO: Respect "Cache-Control: max-age=..."
				data_expires = time::unixSeconds() + (60 * 60 * 24);
				data = std::move(res->body);
				break;
			}
			if (e->second.substr(0, 8) != "https://")
			{
				break;
			}
			e->second.erase(0, 8);
			auto sep = e->second.find('/');
			if (sep == std::string::npos)
			{
				break;
			}
			req.header_fields.at("Host") = e->second.substr(0, sep);
			req.path = e->second.substr(sep);
			req.path_is_encoded = true;
		}
	}

	std::filesystem::path WebResource::getCacheFile() const
	{
		auto file = filesystem::getProgramData();
		file /= "Calamity, Inc";
		file /= "Soup";
		file /= "WebResource";
		std::filesystem::create_directories(file);
		file /= string::bin2hex(sha1::hash(std::string(host).append(path)));
		return file;
	}

	bool WebResource::store()
	{
		if (!hasData())
		{
			return false;
		}
		FileWriter w = getCacheFile();
		return w.u64_dyn(data_expires)
			&& w.str_lp_u64_dyn(data)
			;
	}

	bool WebResource::restore()
	{
		auto file = getCacheFile();
		if (std::filesystem::exists(file))
		{
			FileReader r = getCacheFile();
			return r.u64_dyn(data_expires)
				&& r.str_lp_u64_dyn(data)
				;
		}
		return false;
	}

	void WebResource::downloadWithCaching()
	{
		restore();
		if (!hasData() || isDataExpired())
		{
			download();
			store();
		}
	}

	void WebResource::downloadWithStaleCaching()
	{
		restore();
		if (!hasData())
		{
			download();
			store();
		}
	}
#endif
}
