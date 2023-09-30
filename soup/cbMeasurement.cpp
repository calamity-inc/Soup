#include "cbMeasurement.hpp"

#include "joaat.hpp"
#include "string.hpp"

namespace soup
{
	const char* cbUnitToString(cbUnit unit)
	{
		switch (unit)
		{
		case CB_CM: return "cm";
		case CB_M: return "m";
		case CB_KM: return "km";
		case CB_IN: return "in";
		case CB_FT: return "ft";
		case CB_YARDS: return "yards";
		case CB_RODS: return "rods";
		case CB_CHAINS: return "chains";
		case CB_FURLONGS: return "furlongs";
		case CB_MI: return "miles";

		case CB_G: return "g";
		case CB_KG: return "kg";
		case CB_OUNCES: return "ounces";
		case CB_POUNDS: return "pounds";
		case CB_STONES: return "stones";
		case CB_HUNDREDWEIGHTS: return "hundredweights";
		case CB_USTONS: return "US tons";
		case CB_METRICTONS: return "metric tons";
		case CB_IMPERIALTONS: return "imperial tons";

		case CB_NOUNIT: break;
		}
		SOUP_ASSERT_UNREACHABLE;
	}

	cbUnit cbUnitFromString(std::string str)
	{
		string::lower(str);
		switch (joaat::hash(str))
		{
		case joaat::hash("cm"): return CB_CM;
		case joaat::hash("m"): return CB_M;
		case joaat::hash("km"): return CB_KM;
		case joaat::hash("in"): return CB_IN;
		case joaat::hash("ft"): return CB_FT;
		case joaat::hash("mi"): return CB_MI;
		case joaat::hash("centimetre"): return CB_CM;
		case joaat::hash("centimetres"): return CB_CM;
		case joaat::hash("metre"): return CB_M;
		case joaat::hash("metres"): return CB_M;
		case joaat::hash("kilometre"): return CB_KM;
		case joaat::hash("kilometres"): return CB_KM;
		case joaat::hash("centimeter"): return CB_CM;
		case joaat::hash("centimeters"): return CB_CM;
		case joaat::hash("meter"): return CB_M;
		case joaat::hash("meters"): return CB_M;
		case joaat::hash("kilometer"): return CB_KM;
		case joaat::hash("kilometers"): return CB_KM;
		case joaat::hash("inch"): return CB_IN;
		case joaat::hash("inches"): return CB_IN;
		case joaat::hash("foot"): return CB_FT;
		case joaat::hash("feet"): return CB_FT;
		case joaat::hash("yard"): return CB_YARDS;
		case joaat::hash("yards"): return CB_YARDS;
		case joaat::hash("rod"): return CB_RODS;
		case joaat::hash("rods"): return CB_RODS;
		case joaat::hash("chain"): return CB_CHAINS;
		case joaat::hash("chains"): return CB_CHAINS;
		case joaat::hash("furlong"): return CB_FURLONGS;
		case joaat::hash("furlongs"): return CB_FURLONGS;
		case joaat::hash("mile"): return CB_MI;
		case joaat::hash("miles"): return CB_MI;

		case joaat::hash("g"): return CB_G;
		case joaat::hash("kg"): return CB_KG;
		case joaat::hash("ustons"): return CB_USTONS;
		case joaat::hash("us"): return CB_USTONS;
		case joaat::hash("customarytonnes"): return CB_USTONS;
		case joaat::hash("customarytons"): return CB_USTONS;
		case joaat::hash("customary"): return CB_USTONS;
		case joaat::hash("ton"): return CB_METRICTONS;
		case joaat::hash("tons"): return CB_METRICTONS;
		case joaat::hash("tonne"): return CB_METRICTONS;
		case joaat::hash("tonnes"): return CB_METRICTONS;
		case joaat::hash("metrictons"): return CB_METRICTONS;
		case joaat::hash("metric"): return CB_METRICTONS;
		case joaat::hash("imperialtons"): return CB_IMPERIALTONS;
		case joaat::hash("imperial"): return CB_IMPERIALTONS;
		case joaat::hash("gram"): return CB_G;
		case joaat::hash("grams"): return CB_G;
		case joaat::hash("kilogram"): return CB_KG;
		case joaat::hash("kilograms"): return CB_KG;
		case joaat::hash("kilo"): return CB_KG;
		case joaat::hash("kilos"): return CB_KG;
		case joaat::hash("ounce"): return CB_OUNCES;
		case joaat::hash("ounces"): return CB_OUNCES;
		case joaat::hash("lbs"): return CB_POUNDS;
		case joaat::hash("pound"): return CB_POUNDS;
		case joaat::hash("pounds"): return CB_POUNDS;
		case joaat::hash("stone"): return CB_STONES;
		case joaat::hash("stones"): return CB_STONES;
		case joaat::hash("hundredweight"): return CB_HUNDREDWEIGHTS;
		case joaat::hash("hundredweights"): return CB_HUNDREDWEIGHTS;
		}
		return CB_NOUNIT;
	}

	double cbUnitGetFactor(cbUnit unit) noexcept
	{
		switch (unit)
		{
		case CB_CM: return 0.01;
		case CB_M: break; // return 1.0;
		case CB_KM: return 1000;
		case CB_IN: return 0.0254;
		case CB_FT: return 0.3048;
		case CB_YARDS: return 0.9144;
		case CB_RODS: return 5.029;
		case CB_CHAINS: return 20.117;
		case CB_FURLONGS: return 201.17;
		case CB_MI: return 1609.34;

		case CB_G: return 0.001;
		case CB_KG: break; // return 1.0;
		case CB_OUNCES: return 0.02835;
		case CB_POUNDS: return 0.4535923;
		case CB_STONES: return 6.3502931;
		case CB_HUNDREDWEIGHTS: return 50.8;
		case CB_USTONS: return 907.185;
		case CB_METRICTONS: return 1000;
		case CB_IMPERIALTONS: return 1016.0;

		case CB_NOUNIT: break;
		}
		return 1.0;
	}

	std::string cbMeasurement::toString() const
	{
		std::string str = string::fdecimal(quantity);
		str.push_back(' ');
		str.append(cbUnitToString(unit));
		return str;
	}
}
