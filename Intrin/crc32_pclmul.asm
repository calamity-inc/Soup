.DATA
        ; XMMWORD 0CCAA009E0000000000000000h
        __xmm@00000000ccaa009e0000000000000000 dq 0h, 0CCAA009Eh

        ; XMMWORD 1F70116410000000000000000h
        __xmm@00000001f70116410000000000000000 dq 0h, 1F7011641h

.CODE
        crc32_pclmul PROC
             sub     rsp, 18h
             movdqa xmmword ptr [rsp],xmm6
             movdqu  xmm1, xmmword ptr [rcx]
             not     r8d
             movd    xmm0, r8d
             pxor    xmm0, xmm1
             lea     r10, [rdx-10h]
             cmp     r10, 10h
             jb      loc_533
             add     rdx, 0FFFFFFFFFFFFFFE0h
             mov     r9d, edx
             shr     r9d, 4
             add     r9d, 1
             mov     r8, 1751997D0h
             and     r9, 3
             jz      short loc_485
             shl     r9, 4
             xor     eax, eax
             movq    xmm1, r8
             movdqa  xmm2, xmmword ptr [__xmm@00000000ccaa009e0000000000000000]
             nop     dword ptr [rax+00000000h]
loc_458:
             movdqa  xmm3, xmm0
             pclmulqdq xmm3, xmm1, 0
             movdqu  xmm4, xmmword ptr [rcx+rax+10h]
             pxor    xmm4, xmm3
             pclmulqdq xmm0, xmm2, 11h
             pxor    xmm0, xmm4
             add     rax, 10h
             cmp     r9, rax
             jnz     short loc_458
             add     rcx, rax
             sub     r10, rax
loc_485:
             movdqa  xmm1, xmm0
             cmp     rdx, 30h ; '0'
             jb      loc_537
             add     rcx, 40h ; '@'
             movq    xmm2, r8
             movdqa  xmm3, xmmword ptr [__xmm@00000000ccaa009e0000000000000000]
             nop     dword ptr [rax+00h]
loc_4A8:
             movdqa  xmm4, xmm0
             pclmulqdq xmm4, xmm2, 0
             pclmulqdq xmm0, xmm3, 11h
             movdqu  xmm1, xmmword ptr [rcx-30h]
             pxor    xmm1, xmm4
             pxor    xmm1, xmm0
             movdqu  xmm0, xmmword ptr [rcx-20h]
             movdqu  xmm4, xmmword ptr [rcx-10h]
             movdqu  xmm5, xmmword ptr [rcx]
             movdqa  xmm6, xmm1
             pclmulqdq xmm6, xmm2, 0
             pxor    xmm6, xmm0
             pclmulqdq xmm1, xmm3, 11h
             pxor    xmm1, xmm6
             movdqa  xmm0, xmm1
             pclmulqdq xmm0, xmm2, 0
             pxor    xmm0, xmm4
             pclmulqdq xmm1, xmm3, 11h
             pxor    xmm1, xmm0
             movdqa  xmm0, xmm1
             pclmulqdq xmm0, xmm2, 0
             pxor    xmm0, xmm5
             pclmulqdq xmm1, xmm3, 11h
             pxor    xmm1, xmm0
             add     r10, 0FFFFFFFFFFFFFFC0h
             add     rcx, 40h ; '@'
             movdqa  xmm0, xmm1
             cmp     r10, 0Fh
             ja      loc_4A8
             jmp     short loc_537
loc_533:
             movdqa  xmm1, xmm0
loc_537:
             movdqa  xmm0, xmmword ptr [__xmm@00000000ccaa009e0000000000000000]
             pclmulqdq xmm0, xmm1, 1
             psrldq  xmm1, 8
             pxor    xmm1, xmm0
             pxor    xmm0, xmm0
             pxor    xmm2, xmm2
             movss   xmm2, xmm1
             psrldq  xmm1, 4
             mov     rax, 163CD6124h
             movq    xmm3, rax
             pclmulqdq xmm3, xmm2, 0
             pxor    xmm3, xmm1
             pxor    xmm1, xmm1
             movss   xmm1, xmm3
             pclmulqdq xmm1, xmmword ptr [__xmm@00000001f70116410000000000000000], 10h
             movss   xmm0, xmm1
             mov     rax, 1DB710641h
             movq    xmm1, rax
             pclmulqdq xmm1, xmm0, 0
             pxor    xmm1, xmm3
             pshufd  xmm0, xmm1, 55h ; 'U'
             movd    eax, xmm0
             not     eax
             movaps xmm6, xmmword ptr [rsp]
             add     rsp, 18h
             ret
        crc32_pclmul ENDP
END
