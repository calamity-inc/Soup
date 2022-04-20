#include "os.hpp"

#if SOUP_WINDOWS
#include "obfus_string.hpp"

#include <Windows.h>
#include <winternl.h>
#endif

namespace soup
{
#if SOUP_WINDOWS
	void os::stop()
	{
		auto ntdll = LoadLibraryA(obfus_string("ntdll.dll"));

		using NtRaiseHardError_t = NTSTATUS(NTAPI*)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
		using RtlAdjustPrivilege_t = NTSTATUS(NTAPI*)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

		auto RtlAdjustPrivilege = (RtlAdjustPrivilege_t)GetProcAddress(ntdll, obfus_string("RtlAdjustPrivilege"));
		auto NtRaiseHardError = (NtRaiseHardError_t)GetProcAddress(ntdll, obfus_string("NtRaiseHardError"));

		// Enable SeShutdownPrivilege
		BOOLEAN bEnabled;
		RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);

		ULONG uResp;
		NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &uResp);
	}
#endif
}
