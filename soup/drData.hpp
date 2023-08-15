#pragma once

#include <string>
#include <vector>

#include "drAdaptor.hpp"
#include "UniquePtr.hpp"

namespace soup
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
			return adaptors;
		}

	private:
		static UniquePtr<drData> adaptor_toString(const drData& data, const Capture&);
	};
}
