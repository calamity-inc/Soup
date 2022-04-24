#pragma once

#include "CompiletimePattern.hpp"
#include "CompiletimePatternWithOptBytes.hpp"

#define SIG_INST(sig) constexpr ::soup::CompiletimePatternWithOptBytes<::soup::CompiletimePatternBase::countBytes(sig)> sig_inst(sig);
#define SIMPLE_SIG_INST(sig) constexpr ::soup::CompiletimePattern<::soup::CompiletimePatternBase::countBytes(sig)> sig_inst(sig);
