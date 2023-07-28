#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateFileMappingNumaW,CreateFileMappingNumaW

	.text.windows
        .ftrace1
__CreateFileMappingNuma:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CreateFileMappingNumaW(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__CreateFileMappingNuma,globl
	.previous
