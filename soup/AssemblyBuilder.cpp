#include "AssemblyBuilder.hpp"

#include "base.hpp"

#if SOUP_WINDOWS
#include "AllocRaiiVirtual.hpp"
#else
#include "AllocRaii.hpp"
#endif

#include "UniquePtr.hpp"

namespace soup
{
	void AssemblyBuilder::funcBegin()
	{
		uint8_t bytes[] = {
			0x55, // push rbp
			0x48, 0x89, 0xE5, // mov rbp, rsp
			0x48, 0x83, 0xEC, 0x20, // sub rsp, 20h
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::funcEnd()
	{
		uint8_t bytes[] = {
			0x48, 0x83, 0xC4, 0x20, // add rsp, 20h
			0x5D, // pop rsb
		};
		addBytes(bytes);
		retn();
	}

	void AssemblyBuilder::retn()
	{
		m_data.emplace_back(0xC3);
	}

	void AssemblyBuilder::nop()
	{
		m_data.emplace_back(0x90);
	}

	void AssemblyBuilder::setA(uint64_t val)
	{
		uint8_t bytes[] = {
			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		*(uint64_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void AssemblyBuilder::setD(uint64_t val)
	{
		uint8_t bytes[] = {
			0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		*(uint64_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void AssemblyBuilder::set8(uint64_t val)
	{
		uint8_t bytes[] = {
			0x49, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		*(uint64_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void AssemblyBuilder::set14(uint64_t val)
	{
		uint8_t bytes[] = {
			0x49, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		*(uint64_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void AssemblyBuilder::setAtoC()
	{
		uint8_t bytes[] = {
			0x48, 0x89, 0xC8
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::set12toC()
	{
		uint8_t bytes[] = {
			0x49, 0x89, 0xCC
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::setCto12()
	{
		uint8_t bytes[] = {
			0x4C, 0x89, 0xE1
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::incRAX()
	{
		uint8_t bytes[] = {
			0x48, 0xFF, 0xC0
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::movPtrRAX(uint8_t val)
	{
		uint8_t bytes[] = {
			0xC6, 0x00, val
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::callA()
	{
		uint8_t bytes[] = {
			0xFF, 0xD0
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::jmpA()
	{
		uint8_t bytes[] = {
			0xFF, 0xE0
		};
		addBytes(bytes);
	}

	uint8_t* AssemblyBuilder::data()
	{
		return &m_data.at(0);
	}

	const uint8_t* AssemblyBuilder::data() const
	{
		return &m_data.at(0);
	}

	size_t AssemblyBuilder::size() const noexcept
	{
		return m_data.size();
	}

	UniquePtr<AllocRaiiLocalBase> AssemblyBuilder::allocate() const
	{
#if SOUP_WINDOWS
		UniquePtr<AllocRaiiLocalBase> alloc = soup::make_unique<AllocRaiiVirtual>(size());
#else
		UniquePtr<AllocRaiiLocalBase> alloc = soup::make_unique<AllocRaii>(size());
#endif
		memcpy(alloc->addr, data(), size());
		return alloc;
	}
}
