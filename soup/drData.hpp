#pragma once

#include <string>
#include <vector>

#include "drAdaptor.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	class drData
	{
	public:
		virtual ~drData() = default;

		[[nodiscard]] virtual const char* getTypeName() const noexcept = 0;
		[[nodiscard]] virtual std::string toString() const = 0;

		[[nodiscard]] virtual std::vector<drAdaptor> getAdaptors() const
		{
			std::vector<drAdaptor> adaptors;
			adaptors.emplace_back(drAdaptor{ "string", &adaptor_toString });
			adaptors.emplace_back(drAdaptor{ "typename", &adaptor_getTypeName });
			return adaptors;
		}

	private:
		static UniquePtr<drData> adaptor_toString(const drData& data, const Capture&);
		static UniquePtr<drData> adaptor_getTypeName(const drData& data, const Capture&);
	};
}
