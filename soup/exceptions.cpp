#include "exceptions.hpp"

#if SOUP_WINDOWS
#include <atomic>
#include <sstream>

#include "Thread.hpp"
#include "unicode.hpp"
#endif

NAMESPACE_SOUP
{
#if SOUP_WINDOWS
	static std::atomic<_EXCEPTION_POINTERS*> processing_exp = nullptr;
	static std::string exception_name;

	static SOUP_FORCEINLINE void raise_exp(_EXCEPTION_POINTERS* exp)
	{
		for (_EXCEPTION_POINTERS* expected; expected = nullptr, processing_exp.compare_exchange_weak(expected, exp); );
	}

	// This function can not use any stack space.
	// Luckily, with optimisations, this essentially turns into two tight loops using the respective atomic instructions. At least on x86.
	static long handle_exp(_EXCEPTION_POINTERS* exp)
	{
		raise_exp(exp);
		while (processing_exp.load());
		return EXCEPTION_EXECUTE_HANDLER;
	}
#endif

	Capture exceptions::isolate(Capture(*f)(Capture&&), Capture&& cap)
	{
		Capture ret;

#if SOUP_WINDOWS
		// If we have a stack overflow, we need somewhere to safely handle the exception.
		// Switching to a fiber needs stack space, so we need a thread to hand off to.
		Thread t([](Capture&&)
		{
			while (true)
			{
				if (_EXCEPTION_POINTERS* exp = processing_exp.load())
				{
					if (reinterpret_cast<uintptr_t>(exp) != 1)
					{
						exception_name = exceptions::getName(exp);
						// TODO: We're in a perfect spot to walk the callstack here. Maybe like this: https://gist.github.com/jvranish/4441299#file-stack_traces-c-L84
						processing_exp.store(nullptr);
					}
					break;
				}
				Sleep(1);
			}
		});

		bool is_stk_oflw;
		__try
		{
			ret = f(std::move(cap));
		}
		__except (is_stk_oflw = (GetExceptionInformation()->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW), handle_exp(GetExceptionInformation()))
		{
			if (is_stk_oflw)
			{
				_resetstkoflw();
			}
			SOUP_THROW(osException(std::move(exception_name)));
		}

		raise_exp(reinterpret_cast<EXCEPTION_POINTERS*>(1));
#elif SOUP_POSIX && !SOUP_WASM
		// TODO
#else
		ret = f(std::move(cap));
#endif

		return ret;
	}

#if SOUP_WINDOWS
	[[nodiscard]] static std::string addr2name(const void* addr)
	{
		// TODO: Could use symbol names.
		std::ostringstream oss;
		oss << addr;
		return oss.str();
	}

	static void parseExceptionInformation(std::string& exception_name, ULONG_PTR info[15])
	{
		switch (info[0])
		{
		default:
			exception_name.append(" while ");
			exception_name.append(std::to_string(info[0]));
			exception_name.append(" at ");
			exception_name.append(addr2name((void*)info[1]));
			break;

		case 0:
			exception_name.append(" while reading from ");
			exception_name.append(addr2name((void*)info[1]));
			break;

		case 1:
			exception_name.append(" while writing to ");
			exception_name.append(addr2name((void*)info[1]));
			break;

		case 8:
			exception_name.append(" (DEP at ");
			exception_name.append(addr2name((void*)info[1]));
			exception_name.push_back(')');
			break;
		}
	}

	std::string exceptions::getName(_EXCEPTION_POINTERS* exp)
	{
		std::string exception_name;
		switch (exp->ExceptionRecord->ExceptionCode)
		{
		default:
			{
				std::ostringstream oss;
				oss << std::hex << (uint32_t)exp->ExceptionRecord->ExceptionCode;
				exception_name = oss.str();
			}
			break;

		case 0xE06D7363:
			__try
			{
				exception_name = reinterpret_cast<std::exception*>(exp->ExceptionRecord->ExceptionInformation[1])->what();
				exception_name = unicode::utf16_to_utf8(unicode::acp_to_utf16(exception_name));
				exception_name.insert(0, 1, '"');
				exception_name.push_back('"');
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				exception_name = "C++ exception";
			}
			break;

		case EXCEPTION_ACCESS_VIOLATION:
			exception_name = "Access violation";
			parseExceptionInformation(exception_name, exp->ExceptionRecord->ExceptionInformation);
			break;

		case EXCEPTION_IN_PAGE_ERROR:
			exception_name = "In-page error";
			parseExceptionInformation(exception_name, exp->ExceptionRecord->ExceptionInformation);
			exception_name.append(" (Status ").append(std::to_string(exp->ExceptionRecord->ExceptionInformation[2])).push_back(')');
			break;

		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			exception_name = "Array bounds exceeded";
			break;

		case EXCEPTION_BREAKPOINT:
			exception_name = "Breakpoint";
			break;

		case EXCEPTION_DATATYPE_MISALIGNMENT:
			exception_name = "Data type misalignment";
			break;

		case EXCEPTION_FLT_DENORMAL_OPERAND:
			exception_name = "Float denormal operand";
			break;

		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			exception_name = "Float divide by zero";
			break;

		case EXCEPTION_FLT_INEXACT_RESULT:
			exception_name = "Float inexact result";
			break;

		case EXCEPTION_FLT_INVALID_OPERATION:
			exception_name = "Float invalid operation";
			break;

		case EXCEPTION_FLT_OVERFLOW:
			exception_name = "Float overflow";
			break;

		case EXCEPTION_FLT_STACK_CHECK:
			exception_name = "Float stack check";
			break;

		case EXCEPTION_FLT_UNDERFLOW:
			exception_name = "Float underflow";
			break;

		case EXCEPTION_ILLEGAL_INSTRUCTION:
			exception_name = "Illegal instruction";
			break;

		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			exception_name = "Int divide by zero";
			break;

		case EXCEPTION_INT_OVERFLOW:
			exception_name = "Int overflow";
			break;

		case EXCEPTION_INVALID_DISPOSITION:
			exception_name = "Invalid disposition";
			break;

		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			exception_name = "Noncontinuable exception";
			break;

		case EXCEPTION_PRIV_INSTRUCTION:
			exception_name = "Privileged instruction";
			break;

		case EXCEPTION_SINGLE_STEP:
			exception_name = "Single step";
			break;

		case EXCEPTION_STACK_OVERFLOW:
			exception_name = "Stack overflow";
			break;
		}
		return exception_name;
	}
#endif
}
