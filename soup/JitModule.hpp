#pragma once

#include <filesystem>

#include "SharedLibrary.hpp"

NAMESPACE_SOUP
{
	class JitModule
	{
	private:
		std::string cpp_path;
		std::filesystem::path dll_path;
		SharedLibrary dll;

	public:
		JitModule(std::string cpp_path)
			: cpp_path(std::move(cpp_path))
		{
		}

		~JitModule();

		void remove();

		[[nodiscard]] bool needsToBeCompiled() const;
		std::string compile();
		[[nodiscard]] void* getEntrypoint(const char* name) const noexcept;
	};
}
