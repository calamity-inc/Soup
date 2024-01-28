#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#include <Windows.h>
#include <projectedfslib.h>

#include <filesystem>
#include <vector>

#include "SharedLibrary.hpp"

namespace soup
{
	struct VirtualFilesystem
	{
		struct FileInfo
		{
			std::string name{};
			size_t size{};
			bool is_dir{};
		};

		bool isAvailable();

		[[nodiscard]] bool isMounted() const noexcept { return ctx != nullptr; }
		void mount(const std::filesystem::path& root);
		void unmount();

		[[nodiscard]] virtual std::vector<FileInfo> getDirectoryContents(const std::string& path) = 0;
		virtual void getFileContents(const std::string& path, void* buf, size_t offset, size_t len) noexcept = 0;

		~VirtualFilesystem();

		void operator=(const VirtualFilesystem&) = delete;
		void operator=(VirtualFilesystem&&) = delete;

		SharedLibrary lib;
		decltype(PrjFillDirEntryBuffer)* fpPrjFillDirEntryBuffer;
		decltype(PrjWritePlaceholderInfo)* fpPrjWritePlaceholderInfo;
		PRJ_NAMESPACE_VIRTUALIZATION_CONTEXT ctx = nullptr;

		struct DirectoryEnumeration
		{
			GUID guid;
			std::vector<FileInfo> files{};
			size_t index = 0;
		};
		std::vector<DirectoryEnumeration> dir_enums{};

		[[nodiscard]] DirectoryEnumeration& getDirectoryEnumeration(const GUID& guid)
		{
			for (auto& dir_enum : dir_enums)
			{
				if (dir_enum.guid == guid)
				{
					return dir_enum;
				}
			}
			SOUP_ASSERT_UNREACHABLE;
		}
	};
}
#endif
