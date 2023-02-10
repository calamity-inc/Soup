#include "NativeString.hpp"

#include "base.hpp"

#if SOUP_WINDOWS
#include "unicode.hpp"
#endif

#if SOUP_WINDOWS
static_assert(sizeof(wchar_t) == 2); // UTF-16
#else
static_assert(sizeof(wchar_t) == 4); // UTF-32
#endif

namespace soup
{
	NativeString::NativeString(const wchar_t* str)
		: m_data(str)
	{
	}

	NativeString::NativeString(std::wstring str)
		: m_data(std::move(str))
	{
	}

	wchar_t* NativeString::data() noexcept
	{
		return m_data.data();
	}

	const wchar_t* NativeString::c_str() const noexcept
	{
		return m_data.c_str();
	}


	NativeString::operator std::wstring& () noexcept
	{
		return m_data;
	}

	NativeString::operator const std::wstring& () const noexcept
	{
		return m_data;
	}

	NativeString& NativeString::inc(size_t& off)
	{
		++off;
#if SOUP_WINDOWS
		SOUP_IF_UNLIKELY (UTF16_IS_HIGH_SURROGATE(m_data.at(off - 1)))
		{
			SOUP_IF_LIKELY (off != m_data.size())
			{
				++off;
			}
		}
#endif
		return *this;
	}

	NativeString& NativeString::dec(size_t& off)
	{
		--off;
#if SOUP_WINDOWS
		SOUP_IF_UNLIKELY (UTF16_IS_LOW_SURROGATE(m_data.at(off)))
		{
			SOUP_IF_LIKELY (off != 0)
			{
				--off;
			}
		}
#endif
		return *this;
	}

	size_t NativeString::charLength() const noexcept
	{
#if SOUP_WINDOWS
		return unicode::utf16_char_len(m_data);
#else
		return m_data.size();
#endif
	}

	size_t NativeString::size() const noexcept
	{
		return m_data.size();
	}

	size_t NativeString::length() const noexcept
	{
		return m_data.length();
	}

	NativeString& NativeString::eraseChar(size_t off)
	{
		size_t count = 1;
#if SOUP_WINDOWS
		SOUP_IF_UNLIKELY (UTF16_IS_HIGH_SURROGATE(m_data.at(off)))
		{
			SOUP_IF_LIKELY (off + 1 != m_data.size())
			{
				++count;
			}
		}
#endif
		return eraseBytes(off, count);
	}
}
