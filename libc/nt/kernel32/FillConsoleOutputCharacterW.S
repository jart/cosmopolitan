#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FillConsoleOutputCharacterW,FillConsoleOutputCharacterW,0

#ifdef __x86_64__
	.text.windows
FillConsoleOutputCharacter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FillConsoleOutputCharacterW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	FillConsoleOutputCharacter,globl
	.previous
#endif
