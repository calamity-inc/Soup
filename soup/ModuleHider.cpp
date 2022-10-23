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
				hidden_entry = entry;
				hidden_pNext = entry_ptr;
				*entry_ptr = entry->next;
			}
			entry_ptr = &entry->next;
			entry = *entry_ptr;
		}
	}

	void ModuleHider::disable()
	{
		if (hidden_entry != nullptr)
		{
			reinterpret_cast<LdrDataTableEntry*>(hidden_entry)->next = *reinterpret_cast<LdrDataTableEntry**>(hidden_pNext);
			*reinterpret_cast<LdrDataTableEntry**>(hidden_pNext) = reinterpret_cast<LdrDataTableEntry*>(hidden_entry);

			hidden_entry = nullptr;
		}
	}
}

#endif
