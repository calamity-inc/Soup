#pragma once

#include "fwd.hpp"
#include "szCompressResult.hpp"
#include "szMethod.hpp"
#include "VirtualDtorBase.hpp"

namespace soup
{
	class szCompressor : public VirtualDtorBase
	{
	public:
		[[nodiscard]] static UniquePtr<szCompressor> fromMethod(szMethod method);

		[[nodiscard]] virtual szMethod getMethod() const noexcept = 0;
		[[nodiscard]] virtual bool isByteAligned() const noexcept;
		[[nodiscard]] virtual szCompressResult compress(const std::string& data) const = 0;
		[[nodiscard]] virtual std::string decompress(const BitReader& br) const = 0;
	protected:
		void setLevelOfPreservation(szCompressResult& res, const std::string& data) const;
	};
}
