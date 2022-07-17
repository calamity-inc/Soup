#include "CompiledExecutable.hpp"

#include "Compiler.hpp"

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
		Compiler compiler;
		res.compiler_output = compiler.makeExecutable(path, res.exe_file);
		return res;
    }
}
