#include "rtti.hpp"

#include <cstring> // strlen

namespace soup::rtti
{
	const char* type_info::getMangledName() const noexcept
	{
		return &name[0];
	}

	size_t type_info::getMangledNameLength() const noexcept
	{
		return strlen(getMangledName());
	}

	std::string type_info::getName() const noexcept
	{
		return demangle(getMangledName());
	}

	std::string type_info::demangle(const std::string& str) noexcept
	{
		return demangle(str.c_str());
	}

	std::string type_info::demangle(const char* c) noexcept
	{
		if (*c == '\0')
		{
			return {};
		}

		// in-memory prefices
		if (*c == '.')
		{
			++c;
		}
		if (memcmp(c, "?A", 2) == 0)
		{
			c += 2;
		}

		return demangleNamespace(c);
	}

#define DEMANGLE_LOOP_CHECK_BREAK \
if (*c == '@') \
{ \
	++c; \
	break; \
} \
if (*c == '\0') \
{ \
	break; \
}

	std::string type_info::demangleNamespace(const char*& c) noexcept
	{
		std::string res{};
		while (true)
		{
			auto name = demangleType(c);
			if (res.empty())
			{
				res = name;
			}
			else
			{
				name.append("::");
				res.insert(0, name);
			}
			DEMANGLE_LOOP_CHECK_BREAK;
		}
		return res;
	}

	std::string type_info::demangleType(const char*& c) noexcept
	{
		if (*c == 'V') // class prefix
		{
			++c;
		}
		bool is_template = (memcmp(c, "?$", 2) == 0);
		if (is_template)
		{
			c += 2;
		}
		std::string res = demangleName(c);
		if (is_template)
		{
			res.push_back('<');
			bool more = false;
			while (true)
			{
				if (more)
				{
					res.append(", ");
				}
				res.append(demangleNamespace(c));
				DEMANGLE_LOOP_CHECK_BREAK;
				more = true;
			}
			res.push_back('>');
		}
		return res;
	}

	std::string type_info::demangleName(const char*& c) noexcept
	{
		const char* start = c;
		while (true)
		{
			DEMANGLE_LOOP_CHECK_BREAK;
			++c;
		}
		return std::string(start, c - start - 1);
	}

	object* object::fromVftable(void** vftable) noexcept
	{
		return reinterpret_cast<object*>(vftable[-1]);
	}

	object* object::fromInstance(const void* inst) noexcept
	{
		return fromVftable(*reinterpret_cast<void** const*>(inst));
	}

	uintptr_t object::getImageBase() const noexcept
	{
		return reinterpret_cast<uintptr_t>(this) - this_rva;
	}

	type_info* object::getTypeInfo() const noexcept
	{
		return followRVA<type_info>(type_info_rva);
	}

	hierarchy_info* object::getHierarchyInfo() const noexcept
	{
		return followRVA<hierarchy_info>(hierarchy_info_rva);
	}

	std::string object::toHierarchyString() const noexcept
	{
		std::string res = getTypeInfo()->getName();
		for (auto i = 0; i != getNumParentClasses(); ++i)
		{
			res.append(" : ");
			res.append(getParentClassTypeInfo(i)->getName());
		}
		return res;
	}

	uint32_t object::getNumBaseClasses() const noexcept
	{
		return getHierarchyInfo()->base_classes_size;
	}

	uint32_t* object::getBaseClassArray() const noexcept
	{
		return followRVA<uint32_t>(getHierarchyInfo()->base_classes_rva);
	}

	base_class* object::getBaseClassInfo(uint32_t index) const noexcept
	{
		if (index < getNumBaseClasses())
		{
			return followRVA<base_class>(getBaseClassArray()[index]);
		}
		return nullptr;
	}

	type_info* object::getBaseClassTypeInfo(uint32_t index) const noexcept
	{
		return followRVA<type_info>(getBaseClassInfo(index)->type_info_rva);
	}

	uint32_t object::getNumParentClasses() const noexcept
	{
		return getNumBaseClasses() - 1;
	}

	bool object::hasParentClass() const noexcept
	{
		return getNumParentClasses() > 0;
	}

	type_info* object::getParentClassTypeInfo(uint32_t index) const noexcept
	{
		return getBaseClassTypeInfo(index + 1);
	}
}
