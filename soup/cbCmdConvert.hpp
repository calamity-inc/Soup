#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdConvert : public cbCmd
	{
		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "in", "to" };
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			auto m = p.getArgMeasurementLefthand();
			auto to_unit = p.getArgUnit();
			
			if (!m.isValid()
				|| to_unit == CB_NOUNIT
				)
			{
				std::string msg = m.isValid() ? m.toString() : "...";
				msg.push_back(' ');
				msg.append(*p.command_begin);
				msg.push_back(' ');
				msg.append(to_unit != CB_NOUNIT ? cbUnitToString(to_unit) : "...");
				msg.push_back('?');
				return cbResult(std::move(msg));
			}

			if (cbUnitIsDistance(m.unit))
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

			std::string msg = m.toString();
			msg.push_back(' ');
			msg.append(m.quantity == 1 ? "is" : "are");
			msg.append(" about equal to ");
			msg.append(string::fdecimal(m.getQuantityIn(to_unit)));
			msg.push_back(' ');
			msg.append(cbUnitToString(to_unit));
			return cbResult(std::move(msg));
		}
	};
}
