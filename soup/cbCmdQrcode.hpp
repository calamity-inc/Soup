#pragma once

#include "cbCmd.hpp"

#include "Canvas.hpp"
#include "format.hpp"
#include "QrCode.hpp"

NAMESPACE_SOUP
{
	struct cbCmdQrcode : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"EOR(qr ?code(?:.+"(?'quoted'[^"]+)"|\s+(?'direct'[^\s]+)))EOR");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			std::string to_encode;
			if (auto g = m.findGroupByName("quoted"))
			{
				to_encode = g->toString();
			}
			else
			{
				to_encode = m.findGroupByName("direct")->toString();
			}
			cbResult res(CB_RES_IMAGE, format("Here's a QR code that reads \"{}\".", to_encode));
			res.extra = QrCode::encodeText(to_encode).toCanvas(4, true);
			return res;
		}
	};
}
