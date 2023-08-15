#pragma once

#include "drString.hpp"

#include "JsonObject.hpp"

namespace soup
{
	struct drJsonObject : public drString
	{
		UniquePtr<JsonObject> jsonobject_data;

		drJsonObject(std::string&& string_data, UniquePtr<JsonObject>&& jsonobject_data);

		[[nodiscard]] const char* getTypeName() const noexcept final;
		[[nodiscard]] std::vector<drAdaptor> getAdaptors() const final;

	private:
		static UniquePtr<drData> adaptor_getStringKey(const drData& data, const Capture& cap);
	};
}
