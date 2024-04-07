#include "ModuleHider.hpp"

#if SOUP_WINDOWS && SOUP_BITS == 64

#include "os.hpp"
#include "structing.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif
struct ListEntry
{
	uintptr_t Flink;
	uintptr_t Blink;
};

struct PebLdrData // https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/ntpsapi_x/peb_ldr_data.htm
{
	/* 0x00 */ unsigned long Length;
	PAD(0x04, 0x10) ListEntry InLoadOrderModuleList;
	/* 0x20 */ ListEntry InMemoryOrderModuleList;
	/* 0x30 */ ListEntry InInitializationOrderModuleList;
};
static_assert(sizeof(PebLdrData) == 0x40);

struct LdrDataTableEntry // https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/ntldr/ldr_data_table_entry.htm
{
 	/* 0x00 */ ListEntry InLoadOrderLinks;
	/* 0x10 */ ListEntry InMemoryOrderLinks;
	/* 0x20 */ ListEntry InInitializationOrderLinks;
	/* 0x30 */ void* DllBase;
	PAD(0x38, 0x48) UNICODE_STRING FullDllName;
	/* 0x58 */ UNICODE_STRING BaseDllName;
};
static_assert(sizeof(LdrDataTableEntry) == 0x68);
#ifdef __clang__
#pragma clang diagnostic pop
#endif

NAMESPACE_SOUP
{
	void ModuleHider::enable(HMODULE mod)
	{
		return enable(reinterpret_cast<void*>(mod));
	}

	[[nodiscard]] static LdrDataTableEntry* getLdrDataTableEntryFromLink(uintptr_t link, uintptr_t offset)
	{
		return reinterpret_cast<LdrDataTableEntry*>(link - offset);
	}

	[[nodiscard]] static ListEntry* getListEntryFromLdrDataTableEntry(LdrDataTableEntry* entry, uintptr_t offset)
	{
		return reinterpret_cast<ListEntry*>(reinterpret_cast<uintptr_t>(entry) + offset);
	}

	static void hideInLinkedList(void* base_addr, uintptr_t& hidden_links, uintptr_t*& hidden_pNext, uintptr_t* entry_ptr, uintptr_t offset)
	{
		auto entry = getLdrDataTableEntryFromLink(*entry_ptr, offset);
		while (entry != nullptr
			&& entry->BaseDllName.Buffer != nullptr
			)
		{
			if (entry->DllBase == base_addr)
			{
				hidden_links = *entry_ptr;
				hidden_pNext = entry_ptr;
				*entry_ptr = getListEntryFromLdrDataTableEntry(entry, offset)->Flink;
			}
			entry_ptr = &getListEntryFromLdrDataTableEntry(entry, offset)->Flink;
			entry = getLdrDataTableEntryFromLink(*entry_ptr, offset);
		}
	}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
	void ModuleHider::enable(void* base_addr)
	{
		const auto Ldr = reinterpret_cast<PebLdrData*>(os::getCurrentPeb()->Ldr);
		if (Ldr->Length == 88)
		{
			hideInLinkedList(base_addr, InLoadOrderModuleList_links, InLoadOrderModuleList_pNext, &Ldr->InLoadOrderModuleList.Flink, offsetof(LdrDataTableEntry, InLoadOrderLinks));
			hideInLinkedList(base_addr, InMemoryOrderModuleList_links, InMemoryOrderModuleList_pNext, &Ldr->InMemoryOrderModuleList.Flink, offsetof(LdrDataTableEntry, InMemoryOrderLinks));
			//hideInLinkedList(base_addr, InInitializationOrderModuleList_links, InInitializationOrderModuleList_pNext, &Ldr->InInitializationOrderModuleList.Flink, offsetof(LdrDataTableEntry, InInitializationOrderLinks));
		}
	}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

	void ModuleHider::disable()
	{
		if (InLoadOrderModuleList_links)
		{
			*reinterpret_cast<uintptr_t*>(InLoadOrderModuleList_links) = *InLoadOrderModuleList_pNext;
			*InLoadOrderModuleList_pNext = InLoadOrderModuleList_links;
			InLoadOrderModuleList_links = 0;
		}
		if (InMemoryOrderModuleList_links)
		{
			*reinterpret_cast<uintptr_t*>(InMemoryOrderModuleList_links) = *InMemoryOrderModuleList_pNext;
			*InMemoryOrderModuleList_pNext = InMemoryOrderModuleList_links;
			InMemoryOrderModuleList_links = 0;
		}
		/*if (InInitializationOrderModuleList_links)
		{
			*reinterpret_cast<uintptr_t*>(InInitializationOrderModuleList_links) = *InInitializationOrderModuleList_pNext;
			*InInitializationOrderModuleList_pNext = InInitializationOrderModuleList_links;
			InInitializationOrderModuleList_links = 0;
		}*/
	}
}

#endif
