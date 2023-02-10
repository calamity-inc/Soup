#pragma once

#include <string>

namespace soup
{
	// A codepoint-safe wrapper for std::wstring, so this will store UTF-16 on Windows and UTF-32 on Linux.
	// Obviously all operations are trivial with UTF-32 but there is additional surrogate pair handling for UTF-16.
	class NativeString
	{
	private:
		std::wstring m_data;

	public:
		NativeString(const wchar_t* str);
		NativeString(std::wstring str);

		[[nodiscard]] wchar_t* data() noexcept;
		[[nodiscard]] const wchar_t* c_str() const noexcept;
		operator std::wstring&() noexcept;
		operator const std::wstring&() const noexcept;

		NativeString& inc(size_t& off);
		NativeString& dec(size_t& off);

		[[nodiscard]] size_t charLength() const noexcept;
		[[nodiscard]] size_t size() const noexcept;
		[[nodiscard]] size_t length() const noexcept;

		NativeString& eraseBytes(size_t off = 1, size_t count = std::wstring::npos)
		{
			m_data.erase(off, count);
			return *this;
		}

		NativeString& eraseChar(size_t off);
	};
}
