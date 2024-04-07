#pragma once

#include <filesystem>
#include <string>

#include "base.hpp"

NAMESPACE_SOUP
{
	struct FileRaii
	{
		std::filesystem::path path;

		explicit FileRaii(std::filesystem::path path);
		explicit FileRaii(const FileRaii& b) = delete;
		explicit FileRaii(FileRaii&& b);
		~FileRaii();

		void operator =(const FileRaii& b) = delete;
		void operator =(FileRaii&& b);

		[[nodiscard]] operator const std::filesystem::path&() const noexcept
		{
			return path;
		}

		[[nodiscard]] operator std::string () const noexcept
		{
			return path.string();
		}

		[[nodiscard]] bool exists() const noexcept;

		friend std::ostream& operator<<(std::ostream& os, const FileRaii& v);
	};
}
