#pragma once

#include "base.hpp"

#include <string>

namespace soup
{
	class Mixed
	{
	public:
		enum Type : uint8_t
		{
			NONE,
			INT,
			UINT,
			STRING,
		};

		Type type = NONE;
		uintptr_t val;

		Mixed() = default;

		Mixed(const Mixed& b)
			: type(b.type)
		{
			if (type == STRING)
			{
				val = reinterpret_cast<uintptr_t>(new std::string(b.getString()));
			}
			else
			{
				val = b.val;
			}
		}

		Mixed(Mixed&& b)
			: type(b.type), val(b.type)
		{
			b.type = NONE;
		}

		Mixed(int val)
			: type(INT), val(static_cast<uintptr_t>(val))
		{
		}
		
#if SOUP_BITS > 32
		Mixed(intptr_t val)
			: type(INT), val(static_cast<uintptr_t>(val))
		{
		}
#endif
		
		Mixed(uintptr_t val)
			: type(UINT), val(val)
		{
		}

		Mixed(const char* val)
			: type(STRING), val(reinterpret_cast<uintptr_t>(new std::string(val)))
		{
		}
		
		Mixed(const std::string& val)
			: type(STRING), val(reinterpret_cast<uintptr_t>(new std::string(val)))
		{
		}

		Mixed(std::string val)
			: type(STRING), val(reinterpret_cast<uintptr_t>(new std::string(std::move(val))))
		{
		}

		~Mixed() noexcept
		{
			release();
		}

	private:
		void release()
		{
			if (type == STRING)
			{
				delete reinterpret_cast<std::string*>(val);
			}
		}

	public:
		void reset()
		{
			release();
			type = NONE;
		}

		void operator=(const Mixed& b)
		{
			release();
			type = b.type;
			if (type == STRING)
			{
				val = reinterpret_cast<uintptr_t>(new std::string(b.getString()));
			}
			else
			{
				val = b.val;
			}
		}

		void operator =(Mixed&& b)
		{
			release();
			type = b.type;
			val = b.val;
			b.type = NONE;
		}

		void operator =(int val)
		{
			release();
			this->type = INT;
			this->val = static_cast<uintptr_t>(val);
		}

#if SOUP_BITS > 32
		void operator =(intptr_t val)
		{
			release();
			this->type = INT;
			this->val = static_cast<uintptr_t>(val);
		}
#endif
		
		void operator =(uintptr_t val)
		{
			release();
			this->type = UINT;
			this->val = val;
		}

		void operator =(const std::string& val)
		{
			release();
			this->type = STRING;
			this->val = reinterpret_cast<uintptr_t>(new std::string(val));
		}

		[[nodiscard]] bool isInt() const noexcept
		{
			return type == INT;
		}

		[[nodiscard]] bool isString() const noexcept
		{
			return type == STRING;
		}

		[[nodiscard]] std::string toString() const noexcept
		{
			if (type == INT)
			{
				return std::to_string(static_cast<intptr_t>(val));
			}
			if (type == UINT)
			{
				return std::to_string(val);
			}
			if (type == STRING)
			{
				return *reinterpret_cast<std::string*>(val);
			}
			return {};
		}

		[[nodiscard]] intptr_t getInt() const;
		[[nodiscard]] uintptr_t getUInt() const;
		[[nodiscard]] const std::string& getString() const;
	};
}
