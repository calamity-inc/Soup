#include "InquiryObject.hpp"

#include "Canvas.hpp"
#include "QrCode.hpp"

namespace soup
{
	InquiryObject::InquiryObject(const InquiryObject& b)
		: type(b.type)
	{
		switch (type)
		{
		case QR_CODE:
			cap = b.cap.get<QrCode>();
			break;

		case CANVAS:
			cap = b.cap.get<Canvas>();
			break;
		}
	}

	InquiryObject::InquiryObject(QrCode&& val)
		: type(QR_CODE), cap(std::move(val))
	{
	}

	InquiryObject::InquiryObject(Canvas&& val)
		: type(CANVAS), cap(std::move(val))
	{
	}
}
