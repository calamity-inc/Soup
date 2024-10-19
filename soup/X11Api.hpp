#pragma once

#include "base.hpp"

#include "Exception.hpp"
#include "SharedLibrary.hpp"

NAMESPACE_SOUP
{
	struct X11Api : public SharedLibrary
	{
		// https://codebrowser.dev/kde/include/X11/X.h.html
		using XID = unsigned long;
		using Window = XID;
		using Drawable = XID;
		using Pixmap = XID;
		using KeySym = XID;

		using Display = void;
		using GC = long long;
		using Bool = int;
		using Time = time_t;

		static constexpr Time CurrentTime = 0;

		enum EventMask : long
		{
			KeyPressMask = 0x0000'0001,
			KeyReleaseMask = 0x0000'0002,
			ButtonPressMask = 0x0000'0004,
			ButtonReleaseMask = 0x0000'0008,
			EnterWindowMask = 0x0000'0010,
			LeaveWindowMask = 0x0000'0020,
			PointerMotionMask = 0x0000'0040,
			PointerMotionHintMask = 0x0000'0080,
			Button1MotionMask = 0x0000'0100,
			Button2MotionMask = 0x0000'0200,
			Button3MotionMask = 0x0000'0400,
			Button4MotionMask = 0x0000'0800,
			Button5MotionMask = 0x0000'1000,
			ButtonMotionMask = 0x0000'2000,
			KeymapStateMask = 0x0000'4000,
			ExposureMask = 0x0000'8000,
			VisibilityChangeMask = 0x0001'0000,
			StructureNotifyMask = 0x0002'0000,
			ResizeRedirectMask = 0x0004'0000,
			SubstructureNotifyMask = 0x0008'0000,
			SubstructureRedirectMask = 0x0010'0000,
			FocusChangeMask = 0x0020'0000,
			PropertyChangeMask = 0x0040'0000,
			ColormapChangeMask = 0x0080'0000,
			OwnerGrabButtonMask = 0x010'00000,
		};

		enum EventType : int
		{
			KeyPress = 2,
			KeyRelease,
			ButtonPress,
			ButtonRelease,
			MotionNotify,
			EnterNotify,
			LeaveNotify,
			FocusIn,
			FocusOut,
			KeymapNotify,
			Expose,
			GraphicsExpose,
			NoExpose,
			VisibilityNotify,
			CreateNotify,
			DestroyNotify,
			UnmapNotify,
			MapNotify,
			MapRequest,
			ReparentNotify,
			ConfigureNotify,
			ConfigureRequest,
			GravityNotify,
			ResizeRequest,
			CirculateNotify,
			CirculateRequest,
			PropertyNotify,
			SelectionClear,
			SelectionRequest,
			SelectionNotify,
			ColormapNotify,
			ClientMessage,
			MappingNotify,
		};

		struct XKeyEvent
		{
			int type;		/* KeyPress or KeyRelease */
			unsigned long serial;	/* # of last request processed by server */
			X11Api::Bool send_event;	/* true if this came from a SendEvent request */
			X11Api::Display *display;	/* Display the event was read from */
			X11Api::Window window;		/* ``event'' window it is reported relative to */
			X11Api::Window root;		/* root window that the event occurred on */
			X11Api::Window subwindow;	/* child window */
			X11Api::Time time;		/* milliseconds */
			int x, y;		/* pointer x, y coordinates in event window */
			int x_root, y_root;	/* coordinates relative to root */
			unsigned int state;	/* key or button mask */
			unsigned int keycode;	/* detail */
			X11Api::Bool same_screen;	/* same screen flag */
		};

		struct XFocusChangeEvent
		{
			int type;		/* FocusIn or FocusOut */
			unsigned long serial;	/* # of last request processed by server */
			Bool send_event;	/* true if this came from a SendEvent request */
			Display *display;	/* Display the event was read from */
			Window window;		/* window of event */
			int mode;		/* NotifyNormal, NotifyWhileGrabbed,
						   NotifyGrab, NotifyUngrab */
			int detail;
			/*
			 * NotifyAncestor, NotifyVirtual, NotifyInferior,
			 * NotifyNonlinear,NotifyNonlinearVirtual, NotifyPointer,
			 * NotifyPointerRoot, NotifyDetailNone
			 */
		};

		struct XExposeEvent
		{
			int type;		/* Expose */
			unsigned long serial;	/* # of last request processed by server */
			X11Api::Bool send_event;	/* true if this came from a SendEvent request */
			X11Api::Display *display;	/* Display the event was read from */
			X11Api::Window window;
			int x, y;
			int width, height;
			int count;		/* if nonzero, at least this many more */
		};

		struct XMapEvent
		{
			int type;
			unsigned long serial;	/* # of last request processed by server */
			Bool send_event;	/* true if this came from a SendEvent request */
			Display *display;	/* Display the event was read from */
			Window event;
			Window window;
			Bool override_redirect;	/* boolean, is override set... */
		};

		union XEvent
		{
			int type;
			XKeyEvent xkey;
			XFocusChangeEvent xfocus;
			XExposeEvent xexpose;
			XMapEvent xmap;
			long pad[24];
		};

		using XOpenDisplay_t = Display*(*)(void*);
		//using XDefaultRootWindow_t = Window(*)(Display*);
		using XCreateSimpleWindow_t = Window(*)(Display*, Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int borderWidth, unsigned long border, unsigned long background);
		using XStoreName_t = void(*)(Display*, Window, const char*);
		using XSelectInput_t = void(*)(Display*, Window, long);
		using XMapWindow_t = void(*)(Display*, Window);
		using XNextEvent_t = int(*)(Display*, void*);
		using XLookupKeysym_t = KeySym(*)(XKeyEvent*, int index);

		using XFlush_t = void(*)(Display*);
		//using XSync_t = void(*)(Display*, Bool discard);

		using XCreateGC_t = GC(*)(Display*, Drawable, unsigned long valuemask, void* values);
		using XFreeGC_t = int(*)(Display*, GC gc);
		using XSetForeground_t = void(*)(Display*, GC, unsigned long);
		//using XSetBackground_t = void(*)(Display*, GC, unsigned long);
		//using XSetFillStyle_t = void(*)(Display*, GC, int);
		using XFillRectangle_t = int(*)(Display*, Drawable, GC, int x, int y, unsigned int width, unsigned int height);
		using XCreatePixmap_t = Pixmap(*)(Display*, Drawable, unsigned int width, unsigned int height, unsigned int depth);
		using XFreePixmap_t = void(*)(Display*, Pixmap);
		using XCopyArea_t = void(*)(Display*, Drawable src, Drawable dest, GC gc, int src_x, int src_y, unsigned int width, unsigned int height, int dest_x, int dest_y);

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
		XLookupKeysym_t lookupKeysym;

		XFlush_t flush;
		//XSync_t sync;

		XCreateGC_t createGc;
		XFreeGC_t freeGc;
		XSetForeground_t setForeground;
		//XSetBackground_t setBackground;
		//XSetFillStyle_t setFillStyle;
		XFillRectangle_t fillRectangle;
		XCreatePixmap_t createPixmap;
		XFreePixmap_t freePixmap;
		XCopyArea_t copyArea;

		XDefaultScreen_t defaultScreen;
		XRootWindow_t rootWindow; 

		mutable bool running_message_loop = false;

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
			lookupKeysym = (XLookupKeysym_t)getAddressMandatory("XLookupKeysym");

			flush = (XFlush_t)getAddressMandatory("XFlush");
			//sync = (XSync_t)getAddressMandatory("XSync");

			createGc = (XCreateGC_t)getAddressMandatory("XCreateGC");
			freeGc = (XFreeGC_t)getAddressMandatory("XFreeGC");
			setForeground = (XSetForeground_t)getAddressMandatory("XSetForeground");
			//setBackground = (XSetBackground_t)getAddressMandatory("XSetBackground");
			//setFillStyle = (XSetFillStyle_t)getAddressMandatory("XSetFillStyle");
			fillRectangle = (XFillRectangle_t)getAddressMandatory("XFillRectangle");
			createPixmap = (XCreatePixmap_t)getAddressMandatory("XCreatePixmap");
			freePixmap = (XFreePixmap_t)getAddressMandatory("XFreePixmap");
			copyArea = (XCopyArea_t)getAddressMandatory("XCopyArea");

			defaultScreen = (XDefaultScreen_t)getAddressMandatory("XDefaultScreen");
			rootWindow = (XRootWindow_t)getAddressMandatory("XRootWindow");

			((void(*)())getAddressMandatory("XInitThreads"))();

			display = openDisplay(nullptr); // Note: We're never calling XCloseDisplay
			SOUP_ASSERT(display, "System is headless");

			((int(*)(int(*handler)(Display*)))getAddressMandatory("XSetIOErrorHandler"))([](Display*) -> int
			{
				throw IoError();
			});

			// This disables auto-repeat on the entire X11 server until XAutoRepeatOn is called.
			//((int(*)(Display*))getAddressMandatory("XAutoRepeatOff"))(display);
		}

		struct IoError : public Exception
		{
			IoError()
				: Exception("X11 IO Error")
			{
			}
		};

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
