#pragma once

#include "base.hpp"

#include "SharedLibrary.hpp"

namespace soup
{
	struct X11Api : public SharedLibrary
	{
		using Display = void;
		using Window = unsigned long;

		using XOpenDisplay_t = Display*(*)(void*);
		using XDefaultRootWindow_t = Window(*)(Display*);
		using XCreateSimpleWindow_t = Window(*)(Display*, Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int borderWidth, unsigned long border, unsigned long background);
		using XStoreName_t = void(*)(Display*, Window, const char*);
		using XMapWindow_t = void(*)(Display*, Window);
		using XNextEvent_t = void(*)(Display*, void*);

		Display* display;

		XOpenDisplay_t openDisplay;
		XDefaultRootWindow_t defaultRootWindow;
		XCreateSimpleWindow_t createSimpleWindow;
		XStoreName_t storeName;
		XMapWindow_t mapWindow;
		XNextEvent_t nextEvent;

		X11Api()
			: SharedLibrary("libX11.so.6")
		{
			SOUP_ASSERT(isLoaded());

			openDisplay = (XOpenDisplay_t)getAddress("XOpenDisplay");
			defaultRootWindow = (XDefaultRootWindow_t)getAddress("XDefaultRootWindow");
			createSimpleWindow = (XCreateSimpleWindow_t)getAddress("XCreateSimpleWindow");
			storeName = (XStoreName_t)getAddress("XStoreName");
			mapWindow = (XMapWindow_t)getAddress("XMapWindow");
			nextEvent = (XNextEvent_t)getAddress("XNextEvent");

			display = openDisplay(nullptr);
		}

		[[nodiscard]] static const X11Api& get()
		{
			static X11Api inst;
			return inst;
		}
	};
}
