#include "assembly_builder.hpp"

namespace soup
{
	void assembly_builder::func_begin()
	{
		uint8_t bytes[] = {
			0x55, // push rbp
			0x48, 0x89, 0xE5, // mov rbp, rsp
			0x48, 0x83, 0xEC, 0x20, // sub rsp, 20h
		};
		addBytes(bytes);
	}

	void assembly_builder::func_end()
	{
		uint8_t bytes[] = {
			0x48, 0x83, 0xC4, 0x20, // add rsp, 20h
			0x5D, // pop rsb
		};
		addBytes(bytes);
		retn();
	}

	void assembly_builder::retn()
	{
		m_data.emplace_back(0xC3);
	}

	void assembly_builder::nop()
	{
		m_data.emplace_back(0x90);
	}

	void assembly_builder::setA(uint64_t val)
	{
		uint8_t bytes[] = {
			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		*(uint64_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void assembly_builder::setD(uint64_t val)
	{
		uint8_t bytes[] = {
			0x48, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		*(uint64_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void assembly_builder::set8(uint64_t val)
	{
		uint8_t bytes[] = {
			0x49, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		*(uint64_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void assembly_builder::set14(uint64_t val)
	{
		uint8_t bytes[] = {
			0x49, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		*(uint64_t*)(&bytes[0] + 2) = val;
		addBytes(bytes);
	}

	void assembly_builder::incRAX()
	{
		uint8_t bytes[] = {
			0x48, 0xFF, 0xC0
		};
		addBytes(bytes);
	}

	void assembly_builder::movPtrRAX(uint8_t val)
	{
		uint8_t bytes[] = {
			0xC6, 0x00, val
		};
		addBytes(bytes);
	}

	void assembly_builder::callA()
	{
		uint8_t bytes[] = {
			0xFF, 0xD0
		};
		addBytes(bytes);
	}

	void assembly_builder::jmpA()
	{
		uint8_t bytes[] = {
			0xFF, 0xE0
		};
		addBytes(bytes);
	}

	uint8_t* assembly_builder::data()
	{
		return &m_data.at(0);
	}

	size_t assembly_builder::size() const noexcept
	{
		return m_data.size();
	}
}
