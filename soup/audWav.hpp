#pragma once

#include "base.hpp"

#if SOUP_WINDOWS

#include "audSound.hpp"
#include "Reader.hpp"

NAMESPACE_SOUP
{
	struct audWav : public audSound
	{
		Reader& r;
		uint16_t channels;
		size_t data_begin;
		size_t data_end;

		audWav(Reader& r);

		void prepare() final;
		[[nodiscard]] bool hasFinished() noexcept final;
		double getAmplitude() final;
	};
}

#endif
