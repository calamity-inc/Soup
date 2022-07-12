#include "YubikeyValidator.hpp"

#if !SOUP_WASM

#include <map>

#include "base64.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "rand.hpp"
#include "sha1.hpp"
#include "string.hpp"

namespace soup
{
	YubikeyValidator::YubikeyValidator(const std::string& id, const std::string& secret)
		: base_path("/wsapi/2.0/verify?id="), secret(base64::decode(secret))
	{
		base_path.append(id);
		base_path.append("&otp=");
	}

	YubikeyValidator::Result YubikeyValidator::validate(const std::string& otp) const
	{
		if (otp.size() == 44)
		{
			std::string nonce = soup::rand.str<std::string>(40);

			std::string path = base_path;
			path.append(otp);
			path.append("&nonce=");
			path.append(nonce);
			HttpRequest req{ "GET", "api.yubico.com", std::move(path) };
			auto res = req.execute();
			if (res.has_value())
			{
				std::map<std::string, std::string> kv_map{};
				string::replace_all(res->body, "\r\n", "\n");
				for (const auto& line : string::explode(res->body, '\n'))
				{
					auto sep = line.find('=');
					if (sep != std::string::npos)
					{
						kv_map.emplace(line.substr(0, sep), line.substr(sep + 1));
					}
				}
				if (auto status = kv_map.find("status"); status != kv_map.end())
				{
					if (status->second == "OK")
					{
						if (auto e_otp = kv_map.find("otp"); e_otp != kv_map.end())
						{
							if (e_otp->second == otp)
							{
								if (auto e_nonce = kv_map.find("nonce"); e_nonce != kv_map.end())
								{
									if (e_nonce->second == nonce)
									{
										if (auto h = kv_map.find("h"); h != kv_map.end())
										{
											std::string tbs{};
											for (const auto& e : kv_map)
											{
												if (e.first != "h")
												{
													if (!tbs.empty())
													{
														tbs.push_back('&');
													}
													tbs.append(e.first);
													tbs.push_back('=');
													tbs.append(e.second);
												}
											}
											if (h->second == base64::encode(sha1::hmac(tbs, secret)))
											{
												return { otp.substr(0, 12) };
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		return {};
	}
}

#endif
