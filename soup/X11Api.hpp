#pragma once

#include "base.hpp"

#include "SharedLibrary.hpp"

namespace soup
{
	struct X11Api : public SharedLibrary
	{
		using Display = void;
		using Window = unsigned long;
		using Drawable = Window;
		using GC = long long;
		using Bool = int;

		enum EventMask : long
		{
			ExposureMask = 0x0000'8000,
		};

		enum EventType : int
		{
			Expose = 12,
		};

		using XOpenDisplay_t = Display*(*)(void*);
		//using XDefaultRootWindow_t = Window(*)(Display*);
		using XCreateSimpleWindow_t = Window(*)(Display*, Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int borderWidth, unsigned long border, unsigned long background);
		using XStoreName_t = void(*)(Display*, Window, const char*);
		using XSelectInput_t = void(*)(Display*, Window, long);
		using XMapWindow_t = void(*)(Display*, Window);
		using XNextEvent_t = int(*)(Display*, void*);

		using XFlush_t = void(*)(Display*);
		//using XSync_t = void(*)(Display*, Bool discard);

		using XCreateGC_t = GC(*)(Display*, Drawable, unsigned long valuemask, void* values);
		using XFreeGC_t = int(*)(Display*, GC gc);
		using XSetForeground_t = void(*)(Display*, GC, unsigned long);
		//using XSetBackground_t = void(*)(Display*, GC, unsigned long);
		//using XSetFillStyle_t = void(*)(Display*, GC, int);
		using XFillRectangle_t = int(*)(Display*, Drawable, GC, int x, int y, unsigned int width, unsigned int height);

		using XDefaultScreen_t = int(*)(Display*);
		using XRootWindow_t = Window(*)(Display*, int screen);

		Display* display;

		XOpenDisplay_t openDisplay;
		//XDefaultRootWindow_t defaultRootWindow;
		XCreateSimpleWindow_t createSimpleWindow;
		XStoreName_t storeName;
		XSelectInput_t selectInput;
		XMapWindow_t mapWindow;
		XNextEvent_t nextEvent;

		XFlush_t flush;
		//XSync_t sync;

		XCreateGC_t createGc;
		XFreeGC_t freeGc;
		XSetForeground_t setForeground;
		//XSetBackground_t setBackground;
		//XSetFillStyle_t setFillStyle;
		XFillRectangle_t fillRectangle;

		XDefaultScreen_t defaultScreen;
		XRootWindow_t rootWindow; 

		X11Api()
			: SharedLibrary("libX11.so.6")
		{
			SOUP_ASSERT(isLoaded(), "Failed to load libX11.so.6");

			openDisplay = (XOpenDisplay_t)getAddressMandatory("XOpenDisplay");
			//defaultRootWindow = (XDefaultRootWindow_t)getAddressMandatory("XDefaultRootWindow");
			createSimpleWindow = (XCreateSimpleWindow_t)getAddressMandatory("XCreateSimpleWindow");
			storeName = (XStoreName_t)getAddressMandatory("XStoreName");
			selectInput = (XSelectInput_t)getAddressMandatory("XSelectInput");
			mapWindow = (XMapWindow_t)getAddressMandatory("XMapWindow");
			nextEvent = (XNextEvent_t)getAddressMandatory("XNextEvent");

			flush = (XFlush_t)getAddressMandatory("XFlush");
			//sync = (XSync_t)getAddressMandatory("XSync");

			createGc = (XCreateGC_t)getAddressMandatory("XCreateGC");
			freeGc = (XFreeGC_t)getAddressMandatory("XFreeGC");
			setForeground = (XSetForeground_t)getAddressMandatory("XSetForeground");
			//setBackground = (XSetBackground_t)getAddressMandatory("XSetBackground");
			//setFillStyle = (XSetFillStyle_t)getAddressMandatory("XSetFillStyle");
			fillRectangle = (XFillRectangle_t)getAddressMandatory("XFillRectangle");

			defaultScreen = (XDefaultScreen_t)getAddressMandatory("XDefaultScreen");
			rootWindow = (XRootWindow_t)getAddressMandatory("XRootWindow");

			((void(*)())getAddressMandatory("XInitThreads"))();

			display = openDisplay(nullptr); // Note: We're never calling XCloseDisplay
			SOUP_ASSERT(display, "System is headless");
		}

		[[nodiscard]] static const X11Api& get()
		{
			static X11Api inst;
			return inst;
		}

		/*[[nodiscard]] static constexpr int blackPixel() noexcept
		{
			return 0;
		}

		[[nodiscard]] static constexpr int whitePixel() noexcept
		{
			return 0xFF'FF'FF;
		}*/
	};
}
