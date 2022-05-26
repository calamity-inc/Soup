#include "ModuleHider.hpp"

#if SOUP_WINDOWS && SOUP_BITS == 64

#include "os.hpp"
#include "structing.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
struct LdrDataTableEntry
{
	/* 0x00 */ LdrDataTableEntry* next;
	PAD(0x08, 0x20) void* base_addr;
	PAD(0x28, 0x50) const wchar_t* name;
};
static_assert(sizeof(LdrDataTableEntry) == 0x58);
#pragma clang diagnostic pop

namespace soup
{
	void ModuleHider::enable(HMODULE mod)
	{
		return enable(reinterpret_cast<void*>(mod));
	}

	void ModuleHider::enable(void* base_addr)
	{
		LdrDataTableEntry** entry_ptr = reinterpret_cast<LdrDataTableEntry**>(&os::getCurrentPeb()->Ldr->InMemoryOrderModuleList.Flink);
		LdrDataTableEntry* entry = *entry_ptr;
		while (entry != nullptr && entry->name != nullptr)
		{
			if (entry->base_addr == base_addr)
			{
				ldr_data_table_entry = entry;
				*entry_ptr = entry->next;
			}
			entry_ptr = &entry->next;
			entry = *entry_ptr;
		}
	}

	void ModuleHider::disable()
	{
		if (ldr_data_table_entry != nullptr)
		{
			LdrDataTableEntry* entry = reinterpret_cast<LdrDataTableEntry*>(os::getCurrentPeb()->Ldr->InMemoryOrderModuleList.Flink);
			while (true)
			{
				LdrDataTableEntry* next = entry->next;
				if (next == nullptr || next->name == nullptr)
				{
					entry->next = (LdrDataTableEntry*)ldr_data_table_entry;
					entry->next->next = next;
					break;
				}
				entry = next;
			}
			ldr_data_table_entry = nullptr;
		}
	}
}

#endif
