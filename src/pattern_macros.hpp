#pragma once

#include "pattern_compile_time.hpp"
#include "pattern_compile_time_with_opt_bytes.hpp"

#define SIG_INST(sig) constexpr ::soup::pattern_compile_time_with_opt_bytes<::soup::pattern_compile_time_base::count_bytes(sig)> sig_inst(sig);
#define SIMPLE_SIG_INST(sig) constexpr ::soup::pattern_compile_time<::soup::pattern_compile_time_base::count_bytes(sig)> sig_inst(sig);
