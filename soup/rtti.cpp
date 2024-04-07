#include "rtti.hpp"

#include <cstring> // strlen

NAMESPACE_SOUP
{
	const char* RttiTypeInfo::getMangledName() const noexcept
	{
		return &name[0];
	}

	size_t RttiTypeInfo::getMangledNameLength() const noexcept
	{
		return strlen(getMangledName());
	}

	std::string RttiTypeInfo::getName() const noexcept
	{
		return demangle(getMangledName());
	}

	std::string RttiTypeInfo::demangle(const std::string& str) noexcept
	{
		return demangle(str.c_str());
	}

	std::string RttiTypeInfo::demangle(const char* c) noexcept
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

	std::string RttiTypeInfo::demangleNamespace(const char*& c) noexcept
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

	std::string RttiTypeInfo::demangleType(const char*& c) noexcept
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

	std::string RttiTypeInfo::demangleName(const char*& c) noexcept
	{
		const char* start = c;
		while (true)
		{
			DEMANGLE_LOOP_CHECK_BREAK;
			++c;
		}
		return std::string(start, c - start - 1);
	}

	RttiObject* RttiObject::fromVft(void** vftable) noexcept
	{
		return reinterpret_cast<RttiObject*>(vftable[-1]);
	}

	RttiObject* RttiObject::fromInstance(const void* inst) noexcept
	{
		return fromVft(*reinterpret_cast<void** const*>(inst));
	}

	uintptr_t RttiObject::getImageBase() const noexcept
	{
		return reinterpret_cast<uintptr_t>(this) - this_rva;
	}

	RttiTypeInfo* RttiObject::getTypeInfo() const noexcept
	{
		return followRVA<RttiTypeInfo>(type_info_rva);
	}

	RttiHierarchyInfo* RttiObject::getHierarchyInfo() const noexcept
	{
		return followRVA<RttiHierarchyInfo>(hierarchy_info_rva);
	}

	std::string RttiObject::getHierarchyString() const noexcept
	{
		std::string res = getTypeInfo()->getName();
		for (auto i = 0; i != getNumParentClasses(); ++i)
		{
			res.append(" : ");
			res.append(getParentClassTypeInfo(i)->getName());
		}
		return res;
	}

	uint32_t RttiObject::getNumBaseClasses() const noexcept
	{
		return getHierarchyInfo()->base_classes_size;
	}

	uint32_t* RttiObject::getBaseClassArray() const noexcept
	{
		return followRVA<uint32_t>(getHierarchyInfo()->base_classes_rva);
	}

	RttiBaseClass* RttiObject::getBaseClassInfo(uint32_t index) const noexcept
	{
		if (index < getNumBaseClasses())
		{
			return followRVA<RttiBaseClass>(getBaseClassArray()[index]);
		}
		return nullptr;
	}

	RttiTypeInfo* RttiObject::getBaseClassTypeInfo(uint32_t index) const noexcept
	{
		return followRVA<RttiTypeInfo>(getBaseClassInfo(index)->type_info_rva);
	}

	uint32_t RttiObject::getNumParentClasses() const noexcept
	{
		return getNumBaseClasses() - 1;
	}

	bool RttiObject::hasParentClass() const noexcept
	{
		return getNumParentClasses() > 0;
	}

	RttiTypeInfo* RttiObject::getParentClassTypeInfo(uint32_t index) const noexcept
	{
		return getBaseClassTypeInfo(index + 1);
	}
}
