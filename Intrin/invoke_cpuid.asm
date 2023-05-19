.CODE
	invoke_cpuid PROC
		push rsi
		mov rsi, rcx

		mov eax, edx
		xor ecx, ecx
		cpuid

		mov [rsi], eax
		mov [rsi+4], ebx
		mov [rsi+8], edx
		mov [rsi+12], ecx

		pop rsi
		ret
	invoke_cpuid ENDP
END
