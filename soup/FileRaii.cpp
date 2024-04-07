#include "Tempfile.hpp"

NAMESPACE_SOUP
{
	FileRaii::FileRaii(std::filesystem::path path)
		: path(std::move(path))
	{
	}

	FileRaii::FileRaii(FileRaii&& b)
	{
		operator = (std::move(b));
	}

	FileRaii::~FileRaii()
	{
		if (!path.empty())
		{
			std::filesystem::remove(path);
		}
	}

	void FileRaii::operator=(FileRaii&& b)
	{
		path = std::move(b.path);
		b.path.clear();
	}

	bool FileRaii::exists() const noexcept
	{
		return std::filesystem::exists(path);
	}

	std::ostream& operator<<(std::ostream& os, const FileRaii& v)
	{
		os << v.path;
		return os;
	}
}
