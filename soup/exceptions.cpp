#include "exceptions.hpp"

#include <sstream>

#if SOUP_WINDOWS
#include <atomic>

#include "Thread.hpp"
#include "unicode.hpp"
#elif SOUP_POSIX && !SOUP_WASM
#include <setjmp.h>
#include <signal.h>
#include <string.h> // strsignal
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
#elif SOUP_POSIX && !SOUP_WASM
	static char alt_stack[0x2000];
	static sigjmp_buf jump_buffer;
	static std::string exception_name;

	struct ScopedSignalHandler
	{
		int signum;
		struct sigaction oldact;

		ScopedSignalHandler(int signum, void(*handler)(int, siginfo_t*, void*))
			: signum(signum)
		{
			struct sigaction sa;
			sa.sa_flags = SA_ONSTACK | SA_SIGINFO;
			sa.sa_sigaction = handler;
			sigemptyset(&sa.sa_mask);
			sigaction(signum, &sa, &oldact);
		}

		~ScopedSignalHandler()
		{
			sigaction(signum, &oldact, nullptr);
		}
	};

	static void handle_signal(int signum, siginfo_t* si, void* arg)
	{
		exception_name = strsignal(signum);
		switch (signum)
		{
		case SIGILL:
			switch (si->si_code)
			{
			case ILL_ILLOPC:
				exception_name.append(": Illegal opcode");
				break;

			case ILL_ILLOPN:
				exception_name.append(": Illegal operand");
				break;

			case ILL_ILLADR:
				exception_name.append(": Illegal addressing mode");
				break;

			case ILL_ILLTRP:
				exception_name.append(": Illegal trap");
				break;

			case ILL_PRVOPC:
				exception_name.append(": Privileged opcode");
				break;

			case ILL_PRVREG:
				exception_name.append(": Privileged register");
				break;

			case ILL_COPROC:
				exception_name.append(": Coprocessor error");
				break;

			case ILL_BADSTK:
				exception_name.append(": Internal stack error");
				break;
			}
			exception_name.append(": ").append(exceptions::addr2name(si->si_addr));
			break;

		case SIGFPE:
			switch (si->si_code)
			{
			case FPE_INTDIV:
				exception_name.append(": Integer divide by zero");
				break;

			case FPE_INTOVF:
				exception_name.append(": Integer overflow");
				break;

			case FPE_FLTDIV:
				exception_name.append(": Floating-point divide by zero");
				break;

			case FPE_FLTOVF:
				exception_name.append(": Floating-point overflow");
				break;

			case FPE_FLTUND:
				exception_name.append(": Floating-point underflow");
				break;

			case FPE_FLTRES:
				exception_name.append(": Floating-point inexact result");
				break;

			case FPE_FLTINV:
				exception_name.append(": Floating-point invalid operation");
				break;

			case FPE_FLTSUB:
				exception_name.append(": Subscript out of range");
				break;
			}
			exception_name.append(": ").append(exceptions::addr2name(si->si_addr));
			break;

		case SIGBUS:
			switch (si->si_code)
			{
			case BUS_ADRALN:
				exception_name.append(": Invalid address alignment");
				break;

			case BUS_ADRERR:
				exception_name.append(": Nonexistent physical address");
				break;

			case BUS_OBJERR:
				exception_name.append(": Object-specific hardware error");
				break;

#if SOUP_LINUX
			case BUS_MCEERR_AR:
			case BUS_MCEERR_AO:
				exception_name.append(": Hardware memory error");
				break;
#endif
			}
			exception_name.append(": ").append(exceptions::addr2name(si->si_addr));
			break;

		case SIGSEGV:
			switch (si->si_code)
			{
			case SEGV_MAPERR:
				exception_name.append(": Address not mapped to object");
				break;

			case SEGV_ACCERR:
				exception_name.append(": Invalid permissions for mapped object");
				break;

#if SOUP_LINUX
			case SEGV_BNDERR:
				exception_name.append(": Failed address bound checks");
				break;

			case SEGV_PKUERR:
				exception_name.append(": Access was denied by memory protection keys");
				break;
#endif
			}
			exception_name.append(": ").append(exceptions::addr2name(si->si_addr));
			break;
		}
		siglongjmp(jump_buffer, 1);
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
		stack_t ss = {};
		ss.ss_sp = alt_stack;
		ss.ss_size = sizeof(alt_stack);
		ss.ss_flags = 0;
		sigaltstack(&ss, NULL);

		ScopedSignalHandler _SIGABRT(SIGABRT, &handle_signal);
		ScopedSignalHandler _SIGALRM(SIGALRM, &handle_signal);
		ScopedSignalHandler _SIGBUS(SIGBUS, &handle_signal);
		ScopedSignalHandler _SIGFPE(SIGFPE, &handle_signal);
		ScopedSignalHandler _SIGILL(SIGILL, &handle_signal);
		ScopedSignalHandler _SIGSEGV(SIGSEGV, &handle_signal);
		ScopedSignalHandler _SIGSYS(SIGSYS, &handle_signal);
		ScopedSignalHandler _SIGTERM(SIGTERM, &handle_signal);

		if (sigsetjmp(jump_buffer, 1) == 0)
		{
			ret = f(std::move(cap));
		}
		else
		{
			SOUP_THROW(osException(std::move(exception_name)));
		}
#else
		ret = f(std::move(cap));
#endif

		return ret;
	}

	std::string exceptions::addr2name(const void* addr)
	{
		// TODO: Could use symbol names.
		std::ostringstream oss;
		oss << addr;
		return oss.str();
	}

#if SOUP_WINDOWS
	static void parseExceptionInformation(std::string& exception_name, ULONG_PTR info[15])
	{
		switch (info[0])
		{
		default:
			exception_name.append(" while ");
			exception_name.append(std::to_string(info[0]));
			exception_name.append(" at ");
			exception_name.append(exceptions::addr2name((void*)info[1]));
			break;

		case 0:
			exception_name.append(" while reading from ");
			exception_name.append(exceptions::addr2name((void*)info[1]));
			break;

		case 1:
			exception_name.append(" while writing to ");
			exception_name.append(exceptions::addr2name((void*)info[1]));
			break;

		case 8:
			exception_name.append(" (DEP at ");
			exception_name.append(exceptions::addr2name((void*)info[1]));
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
