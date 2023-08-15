#include "cli.hpp"

#include <iostream>

#include <drString.hpp>
#include <string.hpp>

using namespace soup;

void cli_datareflection()
{
	std::cout << R"(Welcome to the data reflection repl!)" << "\n";
	std::cout << R"(An example snippet: {"date":"2024-04-25T00:00:00Z"} -> date -> year)" << "\n";
	std::cout << R"(Have fun!)" << "\n";
	std::cout << "\n";
	while (true)
	{
		std::cout << "> ";
		std::string line;
		if (!std::getline(std::cin, line))
		{
			break;
		}
		auto arr = string::explode(line, "->");
		UniquePtr<drData> data;
		{
			auto str_data = arr.at(0);
			string::trim(str_data);
			data = drString::reflect(std::move(str_data));
		}
		if (arr.size() == 1)
		{
			arr.emplace_back(std::string{});
		}
		bool transmuted = false;
		for (size_t i = 1; i != arr.size(); ++i)
		{
			const bool last_transmutation = (i + 1 == arr.size());

			std::string target = arr.at(i);
			string::trim(target);

			std::vector<drAdaptor> adaptors = data->getAdaptors();
			const drAdaptor* chosen_adaptor = nullptr;
			for (const auto& adaptor : adaptors)
			{
				if (target == adaptor.name)
				{
					chosen_adaptor = &adaptor;
					break;
				}
			}
			if (chosen_adaptor != nullptr)
			{
				data = (*chosen_adaptor)(*data);
				transmuted = true;
			}
			if (last_transmutation && transmuted)
			{
				std::cout << data->toString() << "\n";
			}
			if (chosen_adaptor == nullptr)
			{
				if (!target.empty())
				{
					std::cout << "Did not find adaptor by name \"" << arr.at(i) << "\"" << "\n";
				}
				std::vector<std::string> adaptor_names{};
				for (const auto& adaptor : adaptors)
				{
					adaptor_names.emplace_back(adaptor.name);
				}
				std::cout << "Adaptors available for this " << data->getTypeName() << ": " << string::join(adaptor_names, ", ") << "\n";
				break;
			}
		}
	}
}
