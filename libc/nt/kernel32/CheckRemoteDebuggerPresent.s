.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CheckRemoteDebuggerPresent,CheckRemoteDebuggerPresent,0

	.text.windows
CheckRemoteDebuggerPresent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CheckRemoteDebuggerPresent(%rip),%rax
	jmp	__sysv2nt
	.endfn	CheckRemoteDebuggerPresent,globl
	.previous
