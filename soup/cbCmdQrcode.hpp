#pragma once

#include "cbCmd.hpp"

#include "Canvas.hpp"
#include "format.hpp"
#include "QrCode.hpp"

namespace soup
{
	struct cbCmdQrcode : public cbCmd
	{
		[[nodiscard]] bool checkTriggers(cbParser& p) const noexcept final
		{
			return p.checkTriggers({ "qrcode", "qr code" });
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			auto to_encode = p.getArgPhrase();
			if (to_encode.empty())
			{
				return cbResult("I'm sorry, I didn't catch what the QR code is supposed to encode.");
			}
			else
			{
				cbResult res(CB_RES_IMAGE, format("Here's a QR code that reads \"{}\".", to_encode));
				res.extra = QrCode::encodeText(to_encode).toCanvas(4, true);
				return res;
			}
		}
	};
}
