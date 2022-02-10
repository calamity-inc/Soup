#include "module.hpp"

#if SOUP_WINDOWS

#include "handle_plain.hpp"
#include "handle_raii.hpp"
#include "memory_buffer.hpp"
#include "pattern.hpp"
#include "pointer.hpp"
#include "alloc_raii_remote.hpp"

namespace soup
{
	module::module(std::unique_ptr<handle_base>&& h, range&& range)
		: h(std::move(h)), m_range(std::move(range))
	{
	}

	module::module(std::unique_ptr<handle_base>&& h)
		: h(std::move(h)), m_range(*this->h, 0)
	{
		auto dosHeader = m_range.base.as<IMAGE_DOS_HEADER*>();
		auto ntHeader = m_range.base.add(dosHeader->e_lfanew).as<IMAGE_NT_HEADERS*>();
		m_range.size = ntHeader->OptionalHeader.SizeOfImage;
	}

	module::module(HANDLE h)
		: module(std::make_unique<handle_plain>(h))
	{
	}

	module::module(std::nullptr_t)
		: module(GetModuleHandle(nullptr))
	{
	}

	module::module(const char* name)
		: module(GetModuleHandleA(name))
	{
	}

	module::operator range() const noexcept
	{
		return m_range;
	}

	pointer module::base() const noexcept
	{
		return m_range.base;
	}

	size_t module::size() const noexcept
	{
		return m_range.size;
	}

	pointer module::getExport(const char* name) const noexcept
	{
		return pointer((void*)GetProcAddress(*h, name));
	}

	size_t module::externalRead(pointer p, void* out, size_t size) const noexcept
	{
		SIZE_T read;
		ReadProcessMemory(*h, p.as<void*>(), out, size, &read);
		return read;
	}

	pointer module::externalScan(const pattern& sig) const
	{
		memory_buffer buf{};
		size_t matched_bytes = 0;
		pointer p = m_range.base;
		size_t end = p.as<uintptr_t>() + m_range.size - 1;
		for (; p.as<size_t>() < end; p = p.add(1))
		{
			if (!buf.covers(p))
			{
				buf.updateRegion(*this, p.as<void*>(), 0x10000);
			}
			if (!sig.m_bytes.at(matched_bytes).has_value() || buf.read<uint8_t>(p) == sig.m_bytes.at(matched_bytes).value())
			{
				if (++matched_bytes >= sig.m_bytes.size())
				{
					return p.sub(sig.m_bytes.size() - 1);
				}
			}
			else
			{
				matched_bytes = 0;
			}
		}
		return nullptr;
	}

	std::unique_ptr<alloc_raii_remote> module::allocate(size_t size, DWORD type, DWORD protect) const
	{
		auto res = std::make_unique<alloc_raii_remote>();
		res->h = *h;
		res->size = size;
		res->p = VirtualAllocEx(*h, nullptr, size, type, protect);
		return res;
	}

	std::unique_ptr<alloc_raii_remote> module::copyInto(const void* data, size_t size) const
	{
		auto res = allocate(size);
		externalWrite(res->p, data, size);
		return res;
	}

	size_t module::externalWrite(pointer p, const void* data, size_t size) const noexcept
	{
		SIZE_T written;
		WriteProcessMemory(*h, p.as<void*>(), data, size, &written);
		return written;
	}

	std::unique_ptr<handle_raii> module::executeAsync(void* rip, uintptr_t rcx) const noexcept
	{
		return std::make_unique<handle_raii>(CreateRemoteThread(*h, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(rip), reinterpret_cast<LPVOID>(rcx), 0, nullptr));
	}

	void module::executeSync(void* rip, uintptr_t rcx) const noexcept
	{
		auto t = executeAsync(rip, rcx);
		WaitForSingleObject(*t, INFINITE);
	}
}

#endif
