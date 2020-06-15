.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_LookupPrivilegeValueW,LookupPrivilegeValueW,1432

	.text.windows
LookupPrivilegeValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LookupPrivilegeValueW(%rip),%rax
	jmp	__sysv2nt
	.endfn	LookupPrivilegeValue,globl
	.previous
