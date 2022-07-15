#include "JitModule.hpp"

#include "Compiler.hpp"
#include "os.hpp"

namespace soup
{
	JitModule::~JitModule()
	{
		remove();
	}

	void JitModule::remove()
	{
		if (!dll_path.empty())
		{
			dll.unload();

			std::filesystem::remove(dll_path);

			auto ext = dll_path.extension().string();
			std::string base = dll_path.string().substr(0, dll_path.string().length() - (ext.length() + 1));

			dll_path.clear();

			std::string lib = base;
			lib.append(".lib");
			std::filesystem::remove(lib);

			std::string exp = base;
			exp.append(".exp");
			std::filesystem::remove(exp);
		}
	}

	bool JitModule::needsToBeCompiled() const noexcept
	{
		return dll_path.empty()
			|| std::filesystem::last_write_time(cpp_path) > std::filesystem::last_write_time(dll_path)
			;
	}

	std::string JitModule::compile()
	{
		dll_path = os::tempfile(Compiler::getSharedLibraryExtension());
		auto output = Compiler::makeSharedLibrary(cpp_path, dll_path.string());
		if (std::filesystem::exists(dll_path))
		{
			dll = SharedLibrary(dll_path.string().c_str());
		}
		else
		{
			remove();
		}
		return output;
	}

	void* JitModule::getEntrypoint(const char* name) const noexcept
	{
		return dll.getAddress(name);
	}
}
