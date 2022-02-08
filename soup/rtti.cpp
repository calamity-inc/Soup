#include "rtti.hpp"

namespace soup::rtti
{
	const char* type_info::getName() const noexcept
	{
		return &name[0];
	}

	size_t type_info::getNameLength() const noexcept
	{
		return strlen(getName());
	}

	std::string type_info::getClassName() const noexcept
	{
		std::string name = getName();
		if (name.substr(0, 4) == ".?AV")
		{
			name.erase(0, 4);
		}
		while (*(name.end() - 1) == '@')
		{
			name.erase(name.end() - 1);
		}
		return name;
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

	uint32_t object::getNumBaseClasses() const noexcept
	{
		return getHierarchyInfo()->base_classes_size;
	}

	base_class** object::getBaseClassArray() const noexcept
	{
		return followRVA<base_class*>(getHierarchyInfo()->base_classes_rva);
	}

	base_class* object::getBaseClassInfo(uint32_t index) const noexcept
	{
		if (index < getNumBaseClasses())
		{
			return getBaseClassArray()[index];
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

	type_info* object::getParentClassTypeInfo() const noexcept
	{
		return getBaseClassTypeInfo(1);
	}
}
