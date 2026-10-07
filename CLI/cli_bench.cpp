#include "cli.hpp"

#include <cstring> // memcmp

#include <aes.hpp>
#include <Benchmark.hpp>
#include <MemoryRefReader.hpp>
#include <rand.hpp>
#include <StringWriter.hpp>
#include <unicode.hpp>

using namespace soup;

static void aes_bench()
{
	BENCHMARK("AES-ECB-128", {
		uint8_t og_data[0x10'000];
		soup::rand.fill(og_data);
		uint8_t data[sizeof(og_data)];
		const char key[] = "Super Secret Key";
		BENCHMARK_LOOP({
			memcpy(data, og_data, sizeof(data));
			aes::ecbEncrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 16);
			aes::ecbDecrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 16);
			SOUP_ASSERT(memcmp(data, og_data, sizeof(data)) == 0);
		});
	});
	BENCHMARK("AES-ECB-192", {
		uint8_t og_data[0x10'000];
		soup::rand.fill(og_data);
		uint8_t data[sizeof(og_data)];
		const char key[] = "Super Secret 192-Bit Key";
		BENCHMARK_LOOP({
			memcpy(data, og_data, sizeof(data));
			aes::ecbEncrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 24);
			aes::ecbDecrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 24);
			SOUP_ASSERT(memcmp(data, og_data, sizeof(data)) == 0);
		});
	});
	BENCHMARK("AES-ECB-256", {
		uint8_t og_data[0x10'000];
		soup::rand.fill(og_data);
		uint8_t data[sizeof(og_data)];
		const char key[] = "My Super Secret Key For 256-Bit";
		BENCHMARK_LOOP({
			memcpy(data, og_data, sizeof(data));
			aes::ecbEncrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 32);
			aes::ecbDecrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 32);
			SOUP_ASSERT(memcmp(data, og_data, sizeof(data)) == 0);
		});
	});
	BENCHMARK("AES-CBC-128", {
		uint8_t og_data[0x10'000];
		soup::rand.fill(og_data);
		uint8_t data[sizeof(og_data)];
		const char key[] = "Super Secret Key";
		const char iv[] = "Super Secret IV";
		BENCHMARK_LOOP({
			memcpy(data, og_data, sizeof(data));
			aes::cbcEncrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 16, reinterpret_cast<const uint8_t*>(iv));
			aes::cbcDecrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 16, reinterpret_cast<const uint8_t*>(iv));
			SOUP_ASSERT(memcmp(data, og_data, sizeof(data)) == 0);
		});
	});
	BENCHMARK("AES-CBC-192", {
		uint8_t og_data[0x10'000];
		soup::rand.fill(og_data);
		uint8_t data[sizeof(og_data)];
		const char key[] = "Super Secret 192-Bit Key";
		const char iv[] = "Super Secret IV";
		BENCHMARK_LOOP({
			memcpy(data, og_data, sizeof(data));
			aes::cbcEncrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 24, reinterpret_cast<const uint8_t*>(iv));
			aes::cbcDecrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 24, reinterpret_cast<const uint8_t*>(iv));
			SOUP_ASSERT(memcmp(data, og_data, sizeof(data)) == 0);
		});
	});
	BENCHMARK("AES-CBC-256", {
		uint8_t og_data[0x10'000];
		soup::rand.fill(og_data);
		uint8_t data[sizeof(og_data)];
		const char key[] = "My Super Secret Key For 256-Bit";
		const char iv[] = "Super Secret IV";
		BENCHMARK_LOOP({
			memcpy(data, og_data, sizeof(data));
			aes::cbcEncrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 32, reinterpret_cast<const uint8_t*>(iv));
			aes::cbcDecrypt(data, sizeof(data), reinterpret_cast<const uint8_t*>(key), 32, reinterpret_cast<const uint8_t*>(iv));
			SOUP_ASSERT(memcmp(data, og_data, sizeof(data)) == 0);
		});
	});
}

static void unicode_bench()
{
	BENCHMARK("utf16_to_utf8_len", {
		UTF16_STRING_TYPE str = UTF16_LITERAL("abcdefghijklmnopqrstuvwxyz アエイオウ あえいおう 💯💯💯");
		BENCHMARK_LOOP({
			SOUP_ASSERT(unicode::utf16_to_utf8_len(str) == 71);
		});
	});
}

#define U64_DYN_BENCH(variant) \
	BENCHMARK(#variant "  7-bit WO", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = 0x7f; \
			sw.variant(x); \
		}); \
	}); \
	BENCHMARK(#variant "  7-bit RW", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = 0x7f; \
			sw.variant(x); \
			MemoryRefReader sr(sw.data); \
			sr.variant(x); \
		}); \
	}); \
	BENCHMARK(#variant "  8-bit WO", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = 0xff; \
			sw.variant(x); \
		}); \
	}); \
	BENCHMARK(#variant "  8-bit RW", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = 0xff; \
			sw.variant(x); \
			MemoryRefReader sr(sw.data); \
			sr.variant(x); \
		}); \
	}); \
	BENCHMARK(#variant " 16-bit WO", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = 0xffff; \
			sw.variant(x); \
		}); \
	}); \
	BENCHMARK(#variant " 16-bit RW", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = 0xffff; \
			sw.variant(x); \
			MemoryRefReader sr(sw.data); \
			sr.variant(x); \
		}); \
	}); \
	BENCHMARK(#variant " 32-bit WO", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = 0xffffffff; \
			sw.variant(x); \
		}); \
	}); \
	BENCHMARK(#variant " 32-bit RW", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = 0xffffffff; \
			sw.variant(x); \
			MemoryRefReader sr(sw.data); \
			sr.variant(x); \
		}); \
	}); \
	BENCHMARK(#variant " 64-bit WO", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = -1; \
			sw.variant(x); \
		}); \
	}); \
	BENCHMARK(#variant " 64-bit RW", { \
		BENCHMARK_LOOP({ \
			StringWriter sw; \
			uint64_t x = -1; \
			sw.variant(x); \
			MemoryRefReader sr(sw.data); \
			sr.variant(x); \
		}); \
	});

void cli_bench()
{
	aes_bench();

	unicode_bench();

	U64_DYN_BENCH(u64_dyn);
	U64_DYN_BENCH(u64_dyn_p);
	U64_DYN_BENCH(u64_dyn_b);
	U64_DYN_BENCH(u64_dyn_bp);
}
