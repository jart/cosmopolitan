#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WriteConsoleOutputCharacterW,WriteConsoleOutputCharacterW,0

#ifdef __x86_64__
	.text.windows
WriteConsoleOutputCharacter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleOutputCharacterW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsoleOutputCharacter,globl
	.previous
#endif
