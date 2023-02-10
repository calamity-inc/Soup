#pragma once

#include <ostream>
#include <string>

#include "unicode.hpp"

namespace soup
{
	// Someone once expressed to me that they don't understand text encoding and there should just be an abstraction that hides it away for them.
	// So, this is that abstraction.
	// However, I can't advise unironic usage of this.
	class OpaqueString
	{
	private:
		std::u32string data;

	public:
		OpaqueString(const char* str) noexcept;
#if SOUP_CPP20
		OpaqueString(const char8_t* str) noexcept;
#endif
		OpaqueString(const std::string& str) noexcept;
		void operator =(const char* str) noexcept;
#if SOUP_CPP20
		void operator =(const char8_t* str) noexcept;
#endif
		void operator =(const std::string& str) noexcept;
		operator std::string() const noexcept;

		OpaqueString(const UTF16_CHAR_TYPE* str) noexcept;
		OpaqueString(const UTF16_STRING_TYPE& str) noexcept;
		void operator =(const UTF16_CHAR_TYPE* str) noexcept;
		void operator =(const UTF16_STRING_TYPE& str) noexcept;
		operator UTF16_STRING_TYPE() const noexcept;

		OpaqueString(const std::u32string& str) noexcept;
		OpaqueString(std::u32string&& str) noexcept;
		void operator =(const std::u32string& str) noexcept;
		void operator =(std::u32string&& str) noexcept;
		operator std::u32string&() noexcept;
		operator const std::u32string&() const noexcept;

		[[nodiscard]] OpaqueString substr(size_t i, size_t l);

		friend std::ostream& operator<<(std::ostream& os, const OpaqueString& v);
	};
}
