#include "SharedLibrary.hpp"

#include "Exception.hpp"

namespace soup
{
	SharedLibrary::SharedLibrary(const std::string& path)
		: SharedLibrary(path.c_str())
	{
	}

	SharedLibrary::SharedLibrary(const char* path)
#if SOUP_WINDOWS
		: handle(LoadLibraryA(path))
#else
		: handle(dlopen(path, RTLD_LAZY))
#endif
	{
	}

	SharedLibrary::SharedLibrary(SharedLibrary&& b)
		: handle(b.handle)
	{
		b.forget();
	}

	SharedLibrary::~SharedLibrary()
	{
		unload();
	}

	void SharedLibrary::operator=(SharedLibrary&& b)
	{
		unload();
		handle = b.handle;
		b.forget();
	}

	bool SharedLibrary::isLoaded() const noexcept
	{
		return handle != nullptr;
	}

	void SharedLibrary::unload()
	{
		if (isLoaded())
		{
#if SOUP_WINDOWS
			FreeLibrary(handle);
#else
			dlclose(handle);
#endif
			forget();
		}
	}

	void SharedLibrary::forget()
	{
		handle = nullptr;
	}

	void* SharedLibrary::getAddress(const char* name) const noexcept
	{
		SOUP_IF_UNLIKELY (!isLoaded())
		{
			return nullptr;
		}
#if SOUP_WINDOWS
		return (void*)GetProcAddress(handle, name);
#else
		return dlsym(handle, name);
#endif
	}

	void* SharedLibrary::getAddressMandatory(const char* name) const
	{
		SOUP_IF_LIKELY (auto addr = getAddress(name))
		{
			return addr;
		}
		std::string msg = "Failed to find mandatory symbol: ";
		msg.append(name);
		SOUP_THROW(Exception(std::move(msg)));
	}
}
