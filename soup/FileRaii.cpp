#include "Tempfile.hpp"

namespace soup
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

	std::ostream& operator<<(std::ostream& os, const FileRaii& v)
	{
		os << v.path;
		return os;
	}
}
