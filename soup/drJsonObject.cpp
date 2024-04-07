#include "drJsonObject.hpp"

#include "drInt.hpp"
#include "JsonInt.hpp"
#include "JsonString.hpp"

NAMESPACE_SOUP
{
	drJsonObject::drJsonObject(std::string&& string_data, UniquePtr<JsonObject>&& jsonobject_data)
		: drString(std::move(string_data)), jsonobject_data(std::move(jsonobject_data))
	{
	}

	const char* drJsonObject::getTypeName() const noexcept
	{
		return "JSON object";
	}

	std::vector<drAdaptor> drJsonObject::getAdaptors() const
	{
		auto adaptors = drString::getAdaptors();
		for (const auto& e : *jsonobject_data)
		{
			if (e.first->isStr())
			{
				std::string key = e.first->reinterpretAsStr().value;
				Capture keycap = std::move(key);
				const char* name = keycap.get<std::string>().c_str();
				adaptors.emplace_back(drAdaptor{ name, &adaptor_getStringKey, std::move(keycap) });
			}
		}
		return adaptors;
	}

	UniquePtr<drData> drJsonObject::adaptor_getStringKey(const drData& data, const Capture& cap)
	{
		const std::string& key = cap.get<std::string>();
		auto node = static_cast<const drJsonObject&>(data).jsonobject_data->find(key);
		if (node->isStr())
		{
			return drString::reflect(node->reinterpretAsStr().value);
		}
		if (node->isInt())
		{
			return soup::make_unique<drInt>(node->reinterpretAsInt().value);
		}
		return soup::make_unique<drString>(node->encode());
	}
}
