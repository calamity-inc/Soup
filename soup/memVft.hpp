#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <cstddef> // size_t

#include "cursed.hpp" // ub_pointer_cast

namespace soup
{
	struct memVft
	{
		void** vft;

		[[nodiscard]] size_t getNumFunctions() const noexcept;
		[[nodiscard]] void* getFunction(size_t i) const noexcept;

#if SOUP_WINDOWS // Because Module is not cross-platfom *yet*
		[[nodiscard]] bool hasRtti() const noexcept;
#endif
		[[nodiscard]] RttiObject* getRtti() const noexcept;

		template <typename Class, typename FuncT>
		[[nodiscard]] static size_t getOffset(FuncT Class::* func) noexcept
		{
			auto flat_func = ub_pointer_cast<void(*)(void*)>(func);

			OffsetTracker dummy;
			flat_func(&dummy);
			return dummy.offset;
		}

		struct OffsetTracker
		{
			virtual void _0() { offset = 0; }
			virtual void _1() { offset = 1; }
			virtual void _2() { offset = 2; }
			virtual void _3() { offset = 3; }
			virtual void _4() { offset = 4; }
			virtual void _5() { offset = 5; }
			virtual void _6() { offset = 6; }
			virtual void _7() { offset = 7; }
			virtual void _8() { offset = 8; }
			virtual void _9() { offset = 9; }
			virtual void _10() { offset = 10; }
			virtual void _11() { offset = 11; }
			virtual void _12() { offset = 12; }
			virtual void _13() { offset = 13; }
			virtual void _14() { offset = 14; }
			virtual void _15() { offset = 15; }
			virtual void _16() { offset = 16; }
			virtual void _17() { offset = 17; }
			virtual void _18() { offset = 18; }
			virtual void _19() { offset = 19; }
			virtual void _20() { offset = 20; }
			virtual void _21() { offset = 21; }
			virtual void _22() { offset = 22; }
			virtual void _23() { offset = 23; }
			virtual void _24() { offset = 24; }
			virtual void _25() { offset = 25; }
			virtual void _26() { offset = 26; }
			virtual void _27() { offset = 27; }
			virtual void _28() { offset = 28; }
			virtual void _29() { offset = 29; }
			virtual void _30() { offset = 30; }
			virtual void _31() { offset = 31; }
			virtual void _32() { offset = 32; }
			virtual void _33() { offset = 33; }
			virtual void _34() { offset = 34; }
			virtual void _35() { offset = 35; }
			virtual void _36() { offset = 36; }
			virtual void _37() { offset = 37; }
			virtual void _38() { offset = 38; }
			virtual void _39() { offset = 39; }
			virtual void _40() { offset = 40; }
			virtual void _41() { offset = 41; }
			virtual void _42() { offset = 42; }
			virtual void _43() { offset = 43; }
			virtual void _44() { offset = 44; }
			virtual void _45() { offset = 45; }
			virtual void _46() { offset = 46; }
			virtual void _47() { offset = 47; }
			virtual void _48() { offset = 48; }
			virtual void _49() { offset = 49; }
			virtual void _50() { offset = 50; }
			virtual void _51() { offset = 51; }
			virtual void _52() { offset = 52; }
			virtual void _53() { offset = 53; }
			virtual void _54() { offset = 54; }
			virtual void _55() { offset = 55; }
			virtual void _56() { offset = 56; }
			virtual void _57() { offset = 57; }
			virtual void _58() { offset = 58; }
			virtual void _59() { offset = 59; }
			virtual void _60() { offset = 60; }
			virtual void _61() { offset = 61; }
			virtual void _62() { offset = 62; }
			virtual void _63() { offset = 63; }
			virtual void _64() { offset = 64; }
			virtual void _65() { offset = 65; }
			virtual void _66() { offset = 66; }
			virtual void _67() { offset = 67; }
			virtual void _68() { offset = 68; }
			virtual void _69() { offset = 69; }
			virtual void _70() { offset = 70; }
			virtual void _71() { offset = 71; }
			virtual void _72() { offset = 72; }
			virtual void _73() { offset = 73; }
			virtual void _74() { offset = 74; }
			virtual void _75() { offset = 75; }
			virtual void _76() { offset = 76; }
			virtual void _77() { offset = 77; }
			virtual void _78() { offset = 78; }
			virtual void _79() { offset = 79; }
			virtual void _80() { offset = 80; }
			virtual void _81() { offset = 81; }
			virtual void _82() { offset = 82; }
			virtual void _83() { offset = 83; }
			virtual void _84() { offset = 84; }
			virtual void _85() { offset = 85; }
			virtual void _86() { offset = 86; }
			virtual void _87() { offset = 87; }
			virtual void _88() { offset = 88; }
			virtual void _89() { offset = 89; }
			virtual void _90() { offset = 90; }
			virtual void _91() { offset = 91; }
			virtual void _92() { offset = 92; }
			virtual void _93() { offset = 93; }
			virtual void _94() { offset = 94; }
			virtual void _95() { offset = 95; }
			virtual void _96() { offset = 96; }
			virtual void _97() { offset = 97; }
			virtual void _98() { offset = 98; }
			virtual void _99() { offset = 99; }

			size_t offset;
		};
	};
}
