#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include "kbRgb.hpp"

#include "IpcSocket.hpp"

NAMESPACE_SOUP
{
	class kbRgbCorsairCue : public kbRgb
	{
	protected:
		std::string id;

		inline static IpcSocket pipe;
	public:
		static bool ensureConnected();
		static void send(const std::string& data);
		[[nodiscard]] static std::string receive();

		kbRgbCorsairCue(std::string&& id, std::string&& model)
			: kbRgb(std::move(model)), id(std::move(id))
		{
			name.insert(0, "Corsair ");
		}

		[[nodiscard]] bool controlsDevice(const hwHid& hid) const noexcept final;

		void deinit() final;

	protected:
		void submitColours(UniquePtr<JsonArray>&& ledColors);

	public:
		void setKey(Key key, Rgb colour) final;
		void setKeys(const Rgb(&colours)[NUM_KEYS]) final;
		void setAllKeys(Rgb colour) final;

		[[nodiscard]] uint8_t getNumColumns() const noexcept final;
		[[nodiscard]] Key mapPosToKey(uint8_t row, uint8_t column) const noexcept final;
	};
}

#endif
