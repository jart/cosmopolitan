#include "libc/nt/codegen.h"
.imp	shell32,__imp_SHGetKnownFolderPath,SHGetKnownFolderPath

	.text.windows
	.ftrace1
SHGetKnownFolderPath:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_SHGetKnownFolderPath(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SHGetKnownFolderPath,globl
	.previous
