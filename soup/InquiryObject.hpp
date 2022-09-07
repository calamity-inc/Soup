#pragma once

#include <cstdint>

#include "fwd.hpp"

#include "Capture.hpp"

namespace soup
{
	struct InquiryObject
	{
		enum Type : uint8_t
		{
			QR_CODE = 0,
			CANVAS,
		};

		Type type;
		Capture cap;

		InquiryObject(const InquiryObject& b);
		InquiryObject(QrCode&& val);
		InquiryObject(Canvas&& val);
	};
}
