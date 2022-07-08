#include "CompiledExecutable.hpp"

#include "base.hpp"

#include "os.hpp"

namespace soup
{
	CompiledExecutable CompiledExecutable::fromCpp(const FileRaii& file)
	{
		return fromCpp(file.path);
	}

	CompiledExecutable CompiledExecutable::fromCpp(const std::filesystem::path& path)
    {
        return fromCpp(path.string());
    }

	CompiledExecutable CompiledExecutable::fromCpp(const char* path)
	{
		return fromCpp(std::string(path));
	}

    CompiledExecutable CompiledExecutable::fromCpp(const std::string& path)
    {
		CompiledExecutable res{
			Tempfile("exe")
		};
		os::execute("clang", {
#if SOUP_WINDOWS
			"-std=c++20",
#else
			"-std=c++17",
			"-lstdc++",
			"-lstdc++fs",
#endif
			"-o", res.exe_file, path
		});
		return res;
    }
}
