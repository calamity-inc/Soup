#include <cstring> // memcpy
#include <iostream>

#include <VirtualFilesystem.hpp>

struct DummyVfs : soup::VirtualFilesystem
{
	std::string dummy_file_contents = "Hello, you're reading a dummy file.";

	[[nodiscard]] std::vector<FileInfo> getDirectoryContents(const std::string& path) final
	{
		std::cout << "Enumerating ./" << path << "\n";
		std::vector<FileInfo> files{};
		if (path.empty() || path.substr(0, 15) == "The Rabbit Hole")
		{
			files.emplace_back(soup::VirtualFilesystem::FileInfo{ "The Rabbit Hole", 0, true });
		}
		if (path.empty())
		{
			files.emplace_back(soup::VirtualFilesystem::FileInfo{ "A Folder", 0, true });
			files.emplace_back(soup::VirtualFilesystem::FileInfo{ "A File.txt", dummy_file_contents.size(), false });
		}
		if (path == "A Folder")
		{
			files.emplace_back(soup::VirtualFilesystem::FileInfo{ "A File In A Folder.txt", dummy_file_contents.size(), false });
		}
		return files;
	}

	size_t getFileContents(const std::string& path, void* buf, size_t offset, size_t len) noexcept final
	{
		std::cout << "Reading ./" << path << " for " << len << " bytes starting at byte " << offset << "\n";
		if (offset >= dummy_file_contents.size())
		{
			std::cout << "File read is out of bounds!\n";
			return 0;
		}
		if (offset + len >= dummy_file_contents.size())
		{
			len = dummy_file_contents.size() - offset;
		}
		memcpy(buf, &dummy_file_contents.at(offset), len);
		return len;
	}
};

int main()
{
	DummyVfs vfs;
	try
	{
		vfs.mount("virt_root");
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::string tmp; std::getline(std::cin, tmp);
		return 1;
	}
	std::cout << "Virtual filesystem is mounted. Press enter to unmount it." << std::endl;;
	std::string tmp; std::getline(std::cin, tmp);
	return 0;
}
