#pragma once

#include <cstdint>
#include <string>

#include "Bitset.hpp"

namespace soup::rtti
{
#pragma pack(push, 1)
	struct type_info
	{
		virtual ~type_info() = default;
		size_t runtime_reference;
		const char name[1]; // this is as long as it needs to be

		[[nodiscard]] const char* getMangledName() const noexcept;
		[[nodiscard]] size_t getMangledNameLength() const noexcept;

		[[nodiscard]] std::string getName() const noexcept;

		[[nodiscard]] static std::string demangle(const std::string& str) noexcept;
		[[nodiscard]] static std::string demangle(const char* c) noexcept;
	private:
		[[nodiscard]] static std::string demangleNamespace(const char*& c) noexcept;
		[[nodiscard]] static std::string demangleType(const char*& c) noexcept;
		[[nodiscard]] static std::string demangleName(const char*& c) noexcept;
	};

	struct hierarchy_info
	{
		uint32_t signature;
		uint32_t attributes;
		uint32_t base_classes_size;
		uint32_t base_classes_rva;
	};

	struct base_class
	{
		uint32_t type_info_rva;
		uint32_t num_parent_classes;
		int32_t member_displacement;
		int32_t vftable_displacement;
		int32_t displacement_within_vftable;
		Bitset<uint32_t> base_class_attributes;
		uint32_t hierarchy_info_rva;
	};

	struct object
	{
		uint32_t signature; // seems to always be 1
		int32_t vftable_offset;
		int32_t constructor_displacement_offset;
		uint32_t type_info_rva;
		uint32_t hierarchy_info_rva;
		uint32_t this_rva;

		[[nodiscard]] static object* fromVftable(void** vftable) noexcept;
		[[nodiscard]] static object* fromInstance(const void* inst) noexcept;

		[[nodiscard]] uintptr_t getImageBase() const noexcept;

		template <typename T>
		[[nodiscard]] T* followRVA(uint32_t rva) const noexcept
		{
			return reinterpret_cast<T*>(getImageBase() + rva);
		}

		[[nodiscard]] type_info* getTypeInfo() const noexcept;
		[[nodiscard]] hierarchy_info* getHierarchyInfo() const noexcept;
		[[nodiscard]] std::string toHierarchyString() const noexcept;

		[[nodiscard]] uint32_t getNumBaseClasses() const noexcept;
		[[nodiscard]] uint32_t* getBaseClassArray() const noexcept;
		[[nodiscard]] base_class* getBaseClassInfo(uint32_t index) const noexcept;
		[[nodiscard]] type_info* getBaseClassTypeInfo(uint32_t index) const noexcept;

		[[nodiscard]] uint32_t getNumParentClasses() const noexcept;
		[[nodiscard]] bool hasParentClass() const noexcept;
		[[nodiscard]] type_info* getParentClassTypeInfo(uint32_t index = 0) const noexcept;
	};
#pragma pack(pop)
}
