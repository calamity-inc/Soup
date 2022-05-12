#include "YubikeyValidator.hpp"

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

	struct CaptureYubikeyValidate
	{
		const YubikeyValidator* validator;
		const std::string* otp;
		std::string nonce;
		YubikeyValidator::Result* res;
	};

	YubikeyValidator::Result YubikeyValidator::validate(const std::string& otp) const
	{
		Result res{};

		if (otp.size() == 44)
		{
			CaptureYubikeyValidate cap;
			cap.validator = this;
			cap.otp = &otp;
			cap.nonce = soup::rand.str<std::string>(40);
			cap.res = &res;

			std::string path = base_path;
			path.append(otp);
			path.append("&nonce=");
			path.append(cap.nonce);
			HttpRequest req{ "GET", "api.yubico.com", std::move(path) };
			req.execute(Callback<void(HttpResponse&&)>([](HttpResponse&& res, Capture&& _cap)
			{
				std::map<std::string, std::string> kv_map{};
				string::replace_all(res.body, "\r\n", "\n");
				for (const auto& line : string::explode(res.body, '\n'))
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
						if (auto otp = kv_map.find("otp"); otp != kv_map.end())
						{
							auto& cap = _cap.get<CaptureYubikeyValidate>();
							if (otp->second == *cap.otp)
							{
								if (auto nonce = kv_map.find("nonce"); nonce != kv_map.end())
								{
									if (nonce->second == cap.nonce)
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
											if (h->second == base64::encode(sha1::hmac(tbs, cap.validator->secret)))
											{
												cap.res->device_id = cap.otp->substr(0, 12);
											}
										}
									}
								}
							}
						}
					}
				}
			}, std::move(cap)));
		}

		return res;
	}
}
