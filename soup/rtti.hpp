#pragma once

#include <cstdint>
#include <string>

#include "Bitset.hpp"

namespace soup
{
#pragma pack(push, 1)
	struct RttiTypeInfo
	{
		virtual ~RttiTypeInfo() = default;
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

	struct RttiHierarchyInfo
	{
		uint32_t signature;
		uint32_t attributes;
		uint32_t base_classes_size;
		uint32_t base_classes_rva;
	};

	struct RttiBaseClass
	{
		uint32_t type_info_rva;
		uint32_t num_parent_classes;
		int32_t member_displacement;
		int32_t vftable_displacement;
		int32_t displacement_within_vftable;
		Bitset<uint32_t> base_class_attributes;
		uint32_t hierarchy_info_rva;
	};

	struct RttiObject
	{
		uint32_t signature; // seems to always be 1
		int32_t vftable_offset;
		int32_t constructor_displacement_offset;
		uint32_t type_info_rva;
		uint32_t hierarchy_info_rva;
		uint32_t this_rva;

		[[nodiscard]] static RttiObject* fromVft(void** vftable) noexcept;
		[[nodiscard]] static RttiObject* fromInstance(const void* inst) noexcept;

		[[nodiscard]] uintptr_t getImageBase() const noexcept;

		template <typename T>
		[[nodiscard]] T* followRVA(uint32_t rva) const noexcept
		{
			return reinterpret_cast<T*>(getImageBase() + rva);
		}

		[[nodiscard]] RttiTypeInfo* getTypeInfo() const noexcept;
		[[nodiscard]] RttiHierarchyInfo* getHierarchyInfo() const noexcept;
		[[nodiscard]] std::string getHierarchyString() const noexcept;

		[[nodiscard]] uint32_t getNumBaseClasses() const noexcept;
		[[nodiscard]] uint32_t* getBaseClassArray() const noexcept;
		[[nodiscard]] RttiBaseClass* getBaseClassInfo(uint32_t index) const noexcept;
		[[nodiscard]] RttiTypeInfo* getBaseClassTypeInfo(uint32_t index) const noexcept;

		[[nodiscard]] uint32_t getNumParentClasses() const noexcept;
		[[nodiscard]] bool hasParentClass() const noexcept;
		[[nodiscard]] RttiTypeInfo* getParentClassTypeInfo(uint32_t index = 0) const noexcept;
	};
#pragma pack(pop)
}
