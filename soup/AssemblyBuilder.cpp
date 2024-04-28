#include "AssemblyBuilder.hpp"

#include "AllocRaiiVirtual.hpp"
#include "os.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
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
			0x5D, // pop rbp
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

	void AssemblyBuilder::setAtoZero()
	{
		uint8_t bytes[] = {
			0x31, 0xC0 // xor eax, eax
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::setA(uint32_t val)
	{
		uint8_t bytes[] = {
			0xB8, 0x00, 0x00, 0x00, 0x00
		};
		*(uint32_t*)(&bytes[0] + 1) = val;
		addBytes(bytes);
	}

	void AssemblyBuilder::setA(uint64_t val)
	{
		uint8_t bytes[] = {
			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		*(uint64_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void AssemblyBuilder::setC(uint64_t val)
	{
		uint8_t bytes[] = {
			0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
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

	void AssemblyBuilder::setAtoU64fromC()
	{
		uint8_t bytes[] = {
			0x48, 0x8B, 0x01
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::setU64fromCtoA()
	{
		uint8_t bytes[] = {
			0x48, 0x89, 0x01
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

	void AssemblyBuilder::cmpRAXtoRDX()
	{
		uint8_t bytes[] = {
			0x48, 0x39, 0xD0
		};
		addBytes(bytes);
	}

	void AssemblyBuilder::cmpPtrRAX(uint8_t val)
	{
		uint8_t bytes[] = {
			0x80, 0x38, 0x00
		};
		*(uint8_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void AssemblyBuilder::je(int8_t offset)
	{
		uint8_t bytes[] = {
			0x74, 0x00
		};
		*(int8_t*)(&bytes[0] + 1) = offset;
		addBytes(bytes);
	}

	void AssemblyBuilder::jne(int8_t offset)
	{
		uint8_t bytes[] = {
			0x75, 0x00
		};
		*(int8_t*)(&bytes[0] + 1) = offset;
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

	UniquePtr<AllocRaiiVirtual> AssemblyBuilder::allocate() const
	{
		return os::allocateExecutable(m_data);
	}
}
