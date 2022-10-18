#include "Uri.hpp"

#include "base64.hpp"
#include "string.hpp"

namespace soup
{
	// URI = scheme ":" ["//" authority] path ["?" query] ["#" fragment]
	// authority = [userinfo "@"] host [":" port]

	Uri::Uri(std::string uri)
	{
		auto scheme_sep = uri.find(':');
		if (scheme_sep != std::string::npos
			&& uri.find("//") > scheme_sep
			)
		{
			scheme = uri.substr(0, scheme_sep);

			uri.erase(0, scheme_sep + 1);
		}

		if (uri.length() > 2 && uri.substr(0, 2) == "//")
		{
			uri.erase(0, 2);

			size_t authority_ends = uri.find('/');
			if (authority_ends == std::string::npos)
			{
				authority_ends = uri.find('?');
				if (authority_ends == std::string::npos)
				{
					authority_ends = uri.find('#');
				}
			}

			auto userinfo_sep = uri.find('@');
			if (userinfo_sep < authority_ends)
			{
				auto pass_sep = uri.find(':');
				if (pass_sep < authority_ends)
				{
					user = uri.substr(0, pass_sep);
					pass = uri.substr((pass_sep + 1), userinfo_sep - (pass_sep + 1));
				}
				else
				{
					user = uri.substr(0, userinfo_sep);
				}

				uri.erase(0, userinfo_sep + 1);
			}

			auto port_sep = uri.find(':');
			if (port_sep < authority_ends)
			{
				host = uri.substr(0, port_sep);
				const char* pPort = &uri.at(port_sep + 1);
				const char* i = pPort;
				port = string::toInt<uint16_t>(i);

				uri.erase(0, port_sep + (i - pPort));
			}
			else
			{
				host = uri.substr(0, authority_ends);

				uri.erase(0, authority_ends);
			}
		}

		auto query_sep = uri.find('?');
		if (query_sep != std::string::npos)
		{
			path = uri.substr(0, query_sep);

			uri.erase(0, query_sep + 1);
			
			auto fragment_sep = uri.find('#');
			if (fragment_sep != std::string::npos)
			{
				query = uri.substr(0, fragment_sep);

				uri.erase(0, fragment_sep + 1);

				fragment = std::move(uri);
			}
			else
			{
				query = std::move(uri);
			}
		}
		else
		{
			auto fragment_sep = uri.find('#');
			if (fragment_sep != std::string::npos)
			{
				path = uri.substr(0, fragment_sep);

				uri.erase(0, fragment_sep + 1);

				fragment = std::move(uri);
			}
			else
			{
				path = std::move(uri);
			}
		}
	}

	std::string Uri::toString() const
	{
		std::string str{};
		if (!scheme.empty())
		{
			str.append(scheme);
			str.push_back(':');
		}
		if (!host.empty())
		{
			str.append("//");
			if (!user.empty())
			{
				str.append(user);
				if (!pass.empty())
				{
					str.push_back(':');
					str.append(pass);
				}
				str.push_back('@');
			}
			str.append(host);
			if (port != 0)
			{
				str.push_back(':');
				str.append(std::to_string(port));
			}
		}
		str.append(getRequestPath());
		if (!fragment.empty())
		{
			str.push_back('#');
			str.append(fragment);
		}
		return str;
	}

	std::string Uri::getRequestPath() const
	{
		auto str = path;
		if (!query.empty())
		{
			str.push_back('?');
			str.append(query);
		}
		return str;
	}

	std::string Uri::data(const char* mime_type, const std::string& contents)
	{
		std::string url = "data:";
		url.append(mime_type);
		url.append(";base64,");
		url.append(base64::encode(contents));
		return url;
	}
}
