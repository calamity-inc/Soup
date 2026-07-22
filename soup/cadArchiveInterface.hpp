#pragma once

#include "cadInterface.hpp"

NAMESPACE_SOUP
{
	struct cadArchiveInterface : public cadInterface
	{
		cadArchive& car;

		cadArchiveInterface(cadArchive& car)
			: car(car)
		{
		}

		std::string getRootHash() override
		{
			return car.root_hash;
		}

		std::string getContent(const std::string& hash) override
		{
			if (auto e = car.objects.find(hash); e != car.objects.end())
			{
				return e->second;
			}
			return {};
		}
	};
}
