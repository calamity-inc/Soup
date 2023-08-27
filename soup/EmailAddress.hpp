#pragma once

namespace soup
{
	struct EmailAddress
	{
		std::string address;
		std::string name;

		EmailAddress(const std::string& str)
		{
			if (auto i = str.find('<'); i != std::string::npos)
			{
				name = str.substr(0, i);
				if (!name.empty())
				{
					name.pop_back();
				}
				address = str.substr(i + 1, str.size() - i - 2);
			}
			else
			{
				address = str;
			}
		}

		[[nodiscard]] std::string toString() const noexcept
		{
			auto str = address;
			str.insert(0, 1, '<');
			str.push_back('>');
			if (!name.empty())
			{
				str.insert(0, 1, ' ');
				str.insert(0, name);
			}
			return str;
		}

		[[nodiscard]] std::string getDomain() const noexcept
		{
			return address.substr(address.find('@') + 1);
		}
	};
}
