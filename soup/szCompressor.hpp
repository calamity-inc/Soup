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
		virtual void compress(BitWriter& bw, const std::string& data) const = 0;
		[[nodiscard]] szCompressResult compress(const std::string& data) const;
		[[nodiscard]] virtual szPreservationLevel getPreservationLevel(const szCompressResult& res, const std::string& data) const;
		[[nodiscard]] virtual std::string decompress(BitReader& br) const = 0;
	protected:
		[[nodiscard]] bool checkDecompressed(const szCompressResult& res, const std::string& data) const;
	};
}
