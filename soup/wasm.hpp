#pragma once

#include "base.hpp"
#include "fwd.hpp"
#include "type_traits.hpp"

#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "math.hpp"
#include "Optional.hpp"
#include "SharedPtr.hpp"
#include "StructMap.hpp"

#ifndef SOUP_WASM_MEMORY64
#define SOUP_WASM_MEMORY64 SOUP_BITS >= 64
#endif

#ifndef SOUP_WASM_MULTI_MEMORY
#define SOUP_WASM_MULTI_MEMORY false
#endif

#ifndef SOUP_WASM_PEDANTIC
// Set to true if you love wasting CPU time just so you can error in edge cases for spec conformity.
#define SOUP_WASM_PEDANTIC false
#endif

NAMESPACE_SOUP
{
	struct WasmVm;

	enum WasmType : uint8_t
	{
		WASM_I32 = 0x7F, // -1
		WASM_I64 = 0x7E, // -2
		WASM_F32 = 0x7D, // -3
		WASM_F64 = 0x7C, // -4
		//WASM_V128 = 0x7B, // -5 (from the simd extension, which is currently not supported by Soup)
		WASM_FUNCREF = 0x70,
		WASM_EXTERNREF = 0x6F,
	};
	[[nodiscard]] WasmType wasm_type_from_string(const std::string& str) noexcept;
	[[nodiscard]] std::string wasm_type_to_string(WasmType type) SOUP_EXCAL;

	struct WasmFunctionType
	{
		std::vector<WasmType> parameters;
		std::vector<WasmType> results;

		[[nodiscard]] bool operator==(const WasmFunctionType& b) const noexcept { return parameters == b.parameters && results == b.results; }
		[[nodiscard]] bool operator!=(const WasmFunctionType& b) const noexcept { return !operator==(b); }

		[[nodiscard]] std::string toString() const SOUP_EXCAL;
	};

	using wasm_ffi_func_t = void(*)(WasmVm&, uint32_t func_index, const WasmFunctionType&);

	struct WasmValue
	{
		union
		{
			struct
			{
				union
				{
					int32_t i32;
					float f32;
				};
				int32_t hi32;
			};
			int64_t i64;
			double f64;
		};
		WasmType type;
		bool mut; // only used for globals

		SOUP_CONSTEXPR20 WasmValue() noexcept : i64(0), type(static_cast<WasmType>(0)) {}
		SOUP_CONSTEXPR20 WasmValue(WasmType type) noexcept : i64(0), type(type) {}
		SOUP_CONSTEXPR20 WasmValue(int32_t i32) noexcept : i32(i32), hi32(0), type(WASM_I32) {}
		SOUP_CONSTEXPR20 WasmValue(uint32_t u32) noexcept : WasmValue(static_cast<int32_t>(u32)) {}
		SOUP_CONSTEXPR20 WasmValue(int64_t i64) noexcept : i64(i64), type(WASM_I64) {}
		SOUP_CONSTEXPR20 WasmValue(uint64_t u64) noexcept : WasmValue(static_cast<int64_t>(u64)) {}
		SOUP_CONSTEXPR20 WasmValue(float f32) noexcept : f32(f32), hi32(0), type(WASM_F32) {}
		SOUP_CONSTEXPR20 WasmValue(double f64) noexcept : f64(f64), type(WASM_F64) {}
		WasmValue(void* ptr) noexcept : i64(reinterpret_cast<uintptr_t>(ptr)), type(WASM_EXTERNREF) {}

		[[nodiscard]] bool operator==(const WasmValue& b) const noexcept { return i64 == b.i64 && type == b.type; }
		[[nodiscard]] bool operator!=(const WasmValue& b) const noexcept { return !operator==(b); }

		[[nodiscard]] size_t uptr() const noexcept
		{
#if SOUP_WASM_MEMORY64
			return i64; // i32 values are guaranteed to have 0 in hi32.
#else
			return i32;
#endif
		}
	};

	struct WasmScript
	{
		struct Memory
		{
			uint8_t* data;
			size_t size;
#if SOUP_WASM_MEMORY64
			uint64_t page_limit : 48;
			uint64_t memory64 : 1;
#else
			uint32_t page_limit;
#endif

			// 1 page = 0x10'000 bytes.
			Memory(uint64_t pages = 0, uint64_t max_pages = 0x10'000, bool _64bit = false) SOUP_EXCAL;
			~Memory() noexcept;

			[[nodiscard]] void* getView(size_t addr, size_t size) noexcept
			{
				SOUP_IF_LIKELY (can_add_without_overflow(addr, size) && addr + size <= this->size)
				{
					return &this->data[addr];
				}
				return nullptr;
			}

			template <typename T>
			[[nodiscard]] T* getPointer(size_t addr) noexcept
			{
				SOUP_IF_LIKELY (auto ptr = getView(addr, sizeof(T)))
				{
					return (T*)ptr;
				}
				return nullptr;
			}

			template <typename T>
			[[nodiscard]] T* getPointer(const WasmValue& base, size_t offset = 0) noexcept
			{
				return getPointer<T>(base.uptr() + offset);
			}

			[[nodiscard]] std::string readString(size_t addr, size_t size) SOUP_EXCAL;
			[[nodiscard]] std::string readNullTerminatedString(size_t addr) SOUP_EXCAL; // a safe alternative to getPointer<const char>(addr)

			bool write(size_t addr, const void* src, size_t size) noexcept;
			bool write(const WasmValue& addr, const void* src, size_t size) noexcept;

			void encodeUPTR(WasmValue& out, size_t in) noexcept;

			[[nodiscard]] WasmType getAddrType() const noexcept
			{
#if SOUP_WASM_MEMORY64
				if (memory64)
				{
					return WASM_I64;
				}
#endif
				return WASM_I32;
			}

			size_t grow(size_t delta_pages) noexcept;
		};

		struct FunctionImport
		{
			std::string module_name;
			std::string function_name;

			wasm_ffi_func_t ptr;
			SharedPtr<WasmScript> source;
			uint32_t type_index; // an index in WasmScript::types of the importing WasmScript
			uint32_t func_index; // to be used with `source` for WASM imports
		};

		enum ImportExportKind : uint8_t
		{
			IE_kFunction = 0,
			IE_kTable = 1,
			IE_kMemory = 2,
			IE_kGlobal = 3,
		};

		struct Import
		{
			std::string module_name;
			std::string field_name;
		};

		struct MemoryImport : public Import
		{
#if SOUP_WASM_MEMORY64
			uint64_t min_pages /*: 48*/;
			uint64_t max_pages : 48;
			uint64_t memory64 : 1;
#else
			uint32_t min_pages;
			uint32_t max_pages;
			bool memory64;
#endif

			MemoryImport(std::string&& module_name, std::string&& field_name, uint64_t min_pages, uint64_t max_pages, bool _64bit) noexcept
				: Import{ std::move(module_name), std::move(field_name) }, min_pages(min_pages), max_pages(max_pages)
#if SOUP_WASM_MEMORY64
				, memory64(_64bit)
#endif
			{
#if !SOUP_WASM_MEMORY64
				SOUP_UNUSED(_64bit);
#endif
			}

			[[nodiscard]] bool isCompatibleWith(const Memory& mem) const noexcept;
		};

		struct GlobalImport : public Import
		{
			WasmType type;
			bool mut;
		};

		struct Export
		{
			uint8_t kind;
			uint32_t index;
		};

		struct DataSegment
		{
			uint8_t base[12];
			uint32_t memidx;
			std::string data;

			[[nodiscard]] bool isPassive() const noexcept { return memidx == -1; }
		};

		struct Table
		{
			const WasmType type;
#if SOUP_WASM_MEMORY64
			bool table64 = false;
#else
			uint32_t limit;
#endif
			std::vector<uint64_t> values;
#if SOUP_WASM_MEMORY64
			uint64_t limit;
#endif

			Table(WasmType type) noexcept
				: type(type), limit(0x10'000)
			{
			}

			size_t grow(size_t delta, int64_t value = 0) SOUP_EXCAL;
			bool copy(const Table& src, size_t dst_offset, size_t src_offset, size_t size) noexcept;
		};

#if SOUP_WASM_MULTI_MEMORY
		std::vector<SharedPtr<Memory>> memories;
		std::vector<MemoryImport> memory_imports;
#else
		SharedPtr<Memory> memory;
		Optional<MemoryImport> memory_import;
#endif
		std::vector<uint32_t> functions{}; // (function_index - function_imports.size()) -> type_index
		std::vector<WasmFunctionType> types{};
		std::vector<FunctionImport> function_imports{};
		std::vector<GlobalImport> global_imports{};
		std::vector<SharedPtr<WasmValue>> globals{};
		std::unordered_map<std::string, Export> export_map{};
		std::vector<std::string> code{};
		std::unordered_map<uint64_t, std::vector<uint32_t>> _internal_branch_hints{};
		std::vector<Table> tables{};
		std::vector<DataSegment> data_segments{};
		std::unordered_map<uint32_t, Table> passive_elem_segments{};
		StructMap custom_data;
		uint32_t start_func_idx = -1;

		WasmScript() noexcept { /* default */ }
		WasmScript(WasmScript&&) noexcept = default;
		WasmScript(const WasmScript&) = delete;
		WasmScript& operator = (WasmScript&&) noexcept = default;
		WasmScript& operator = (const WasmScript&) = delete;

		bool load(const std::string& data) SOUP_EXCAL;
		bool load(Reader& r) SOUP_EXCAL;
		bool readConstantExpression(Reader& r, std::string& out) SOUP_EXCAL;
		bool readConstant(Reader& r, WasmValue& out) noexcept;
		bool validateFunctionBody(Reader& r) noexcept;

		[[nodiscard]] bool hasUnresolvedImports() const noexcept;
		void provideImportedFunction(const std::string& module_name, const std::string& function_name, wasm_ffi_func_t ptr) noexcept;
		void provideImportedFunctions(const std::string& module_name, const std::unordered_map<std::string, wasm_ffi_func_t>& map) noexcept;
		void provideImportedGlobal(const std::string& module_name, const std::string& field_name, SharedPtr<WasmValue> value) noexcept;
		void provideImportedGlobals(const std::string& module_name, const std::unordered_map<std::string, SharedPtr<WasmValue>>& map) noexcept;
		void provideImportedMemory(const std::string& module_name, const std::string& field_name, SharedPtr<Memory> value) noexcept;
		void importFromModule(const std::string& module_name, SharedPtr<WasmScript> other) noexcept;
		void linkWasiPreview1(std::vector<std::string> args = {}) noexcept;

		// Runs the start function of the script, if defined. May throw if an imported C++ function throws.
		bool instantiate();

		[[nodiscard]] const std::string* getExportedFuntion(const std::string& name, const WasmFunctionType** optOutType = nullptr) const noexcept;
		[[nodiscard]] uint32_t getExportedFuntion2(const std::string& name) const noexcept;
		[[nodiscard]] uint32_t getTypeIndexForFunction(uint32_t func_index) const noexcept;
		[[nodiscard]] WasmValue* getGlobalByIndex(uint32_t global_index) noexcept { return global_index < globals.size() ? globals[global_index].get()  : nullptr; }
		[[nodiscard]] WasmValue* getExportedGlobal(const std::string& name) noexcept;
#if SOUP_WASM_MULTI_MEMORY
		[[nodiscard]] Memory* getMemoryByIndex(uint32_t memidx) noexcept { return memidx < memories.size() ? memories[memidx].get() : nullptr; }
#else
		[[nodiscard]] Memory* getMemoryByIndex(uint32_t memidx) noexcept { return memidx == 0 ? memory.get() : nullptr; }
#endif
		[[nodiscard]] std::string* getPassiveDataSegmentByIndex(uint32_t i) noexcept;

		// May throw if an imported C++ function throws.
		bool call(uint32_t func_index, std::vector<WasmValue>&& args = {}, std::vector<WasmValue>* out = nullptr);
	};

	struct WasmVm
	{
		std::vector<WasmValue> stack;
		std::vector<WasmValue> locals;
		WasmScript& script;

		WasmVm(WasmScript& script) noexcept
			: script(script)
		{
		}

		// May throw if an imported C++ function throws.
		bool run(const std::string& data, unsigned depth = 0, uint32_t func_index = -1);
		bool run(Reader& r, unsigned depth = 0, uint32_t func_index = -1);

		struct CtrlFlowEntry
		{
			std::streamoff position; // -1 for forward jumps
			size_t stack_size;
			uint32_t num_values; // Number of values to keep on the stack top after branching. num_results for forward jumps; num_params for backward jumps.
		};

		static bool skipOverBranch(Reader& r, uint32_t depth, WasmScript& script, uint32_t func_index) SOUP_EXCAL;
		[[nodiscard]] bool doBranch(Reader& r, uint32_t depth, uint32_t func_index, std::stack<CtrlFlowEntry>& ctrlflow) SOUP_EXCAL;
		[[nodiscard]] bool doCall(uint32_t type_index, uint32_t function_index, unsigned depth = 0);
	};

	struct WasmScrapAllocator
	{
		WasmScript::Memory& memory;
		size_t last_alloc = -1;

		WasmScrapAllocator(WasmScript::Memory& memory) noexcept
			: memory(memory)
		{
		}

		[[nodiscard]] size_t allocate(size_t len) noexcept
		{
			if (last_alloc >= memory.size)
			{
				last_alloc = memory.size - 1;
			}
			last_alloc -= len;
			return last_alloc;
		}
	};
}
