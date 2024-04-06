#pragma once

#include "cbCmd.hpp"

#include "cbMeasurement.hpp"

namespace soup
{
	struct cbCmdConvert : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"((?'fromqty'[0-9.]+)\s*(?'fromunit'cm|m|km|in|ft|mi|centimetre|centimetres|metre|metres|kilometre|kilometres|centimeter|centimeters|meter|meters|kilometer|kilometers|inch|inches|foot|feet|yard|yards|rod|rods|chain|chains|furlong|furlongs|mile|miles|g|kg|ustons|us|customarytonnes|customarytons|customary|ton|tons|tonne|tonnes|metrictons|metric|imperialtons|imperial|gram|grams|kilogram|kilograms|kilo|kilos|ounce|ounces|lbs|pound|pounds|stone|stones|hundredweight|hundredweights)\s+(?:in|to)\s+(?'tounit'cm|m|km|in|ft|mi|centimetre|centimetres|metre|metres|kilometre|kilometres|centimeter|centimeters|meter|meters|kilometer|kilometers|inch|inches|foot|feet|yard|yards|rod|rods|chain|chains|furlong|furlongs|mile|miles|g|kg|ustons|us|customarytonnes|customarytons|customary|ton|tons|tonne|tonnes|metrictons|metric|imperialtons|imperial|gram|grams|kilogram|kilograms|kilo|kilos|ounce|ounces|lbs|pound|pounds|stone|stones|hundredweight|hundredweights)\b)");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			cbMeasurement from;
			char* str_end;
			from.quantity = std::strtod(m.findGroupByName("fromqty")->begin, &str_end);
			if (str_end == m.findGroupByName("fromqty")->begin || from.quantity == HUGE_VAL)
			{
				return "Invalid number";
			}
			from.unit = cbUnitFromString(m.findGroupByName("fromunit")->toString());
			auto to_unit = cbUnitFromString(m.findGroupByName("tounit")->toString());

			if (cbUnitIsDistance(from.unit))
			{
				if (!cbUnitIsDistance(to_unit))
				{
					return "I'm not quite sure how to convert a distance to a weight.";
				}
			}
			else //if (cbUnitIsWeight(m.unit))
			{
				if (!cbUnitIsWeight(to_unit))
				{
					return "I'm not quite sure how to convert a weight to a distance.";
				}
			}

			std::string msg = from.toString();
			msg.push_back(' ');
			msg.append(from.quantity == 1 ? "is" : "are");
			msg.append(" about equal to ");
			msg.append(string::fdecimal(from.getQuantityIn(to_unit)));
			msg.push_back(' ');
			msg.append(cbUnitToString(to_unit));
			return cbResult(std::move(msg));
		}
	};
}
