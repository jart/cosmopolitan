#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WriteConsoleOutputCharacterW,WriteConsoleOutputCharacterW,0

	.text.windows
WriteConsoleOutputCharacter:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleOutputCharacterW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WriteConsoleOutputCharacter,globl
	.previous
