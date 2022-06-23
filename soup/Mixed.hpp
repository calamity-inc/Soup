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
		uint64_t val;

		Mixed() = default;

		Mixed(const Mixed& b)
			: type(b.type)
		{
			if (type == STRING)
			{
				val = (uint64_t)new std::string(b.getString());
			}
			else
			{
				val = b.val;
			}
		}

		Mixed(Mixed&& b)
			: type(b.type), val(b.val)
		{
			b.type = NONE;
		}

		Mixed(int32_t val)
			: type(INT), val((uint64_t)val)
		{
		}
		
		Mixed(int64_t val)
			: type(INT), val((uint64_t)val)
		{
		}
		
		Mixed(uint64_t val)
			: type(UINT), val(val)
		{
		}

		Mixed(const char* val)
			: type(STRING), val((uint64_t)new std::string(val))
		{
		}
		
		Mixed(const std::string& val)
			: type(STRING), val((uint64_t)new std::string(val))
		{
		}

		Mixed(std::string&& val)
			: type(STRING), val((uint64_t)new std::string(std::move(val)))
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

		void operator =(const Mixed& b)
		{
			release();
			type = b.type;
			if (type == STRING)
			{
				val = (uint64_t)new std::string(b.getString());
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

		void operator =(int32_t val)
		{
			release();
			this->type = INT;
			this->val = (uint64_t)val;
		}

		void operator =(int64_t val)
		{
			release();
			this->type = INT;
			this->val = (uint64_t)val;
		}
		
		void operator =(uint64_t val)
		{
			release();
			this->type = UINT;
			this->val = val;
		}

		void operator =(const std::string& val)
		{
			release();
			this->type = STRING;
			this->val = (uint64_t)new std::string(val);
		}

		void operator =(std::string&& val)
		{
			release();
			this->type = STRING;
			this->val = (uint64_t)new std::string(std::move(val));
		}

		[[nodiscard]] bool isInt() const noexcept
		{
			return type == INT;
		}

		[[nodiscard]] bool isUInt() const noexcept
		{
			return type == UINT;
		}

		[[nodiscard]] bool isString() const noexcept
		{
			return type == STRING;
		}

		[[nodiscard]] std::string toString() const noexcept
		{
			if (type == INT)
			{
				return std::to_string((int64_t)val);
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

		friend std::ostream& operator<<(std::ostream& os, const Mixed& v);

		[[nodiscard]] int64_t getInt() const;
		[[nodiscard]] uint64_t getUInt() const;
		[[nodiscard]] std::string& getString() const;
	};
}
