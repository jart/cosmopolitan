.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CheckRemoteDebuggerPresent,CheckRemoteDebuggerPresent,125

	.text.windows
CheckRemoteDebuggerPresent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CheckRemoteDebuggerPresent(%rip),%rax
	jmp	__sysv2nt
	.endfn	CheckRemoteDebuggerPresent,globl
	.previous
