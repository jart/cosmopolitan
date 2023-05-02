#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CheckRemoteDebuggerPresent,CheckRemoteDebuggerPresent,0

#ifdef __x86_64__
	.text.windows
CheckRemoteDebuggerPresent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CheckRemoteDebuggerPresent(%rip),%rax
	jmp	__sysv2nt
	.endfn	CheckRemoteDebuggerPresent,globl
	.previous
#endif
