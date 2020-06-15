.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_LookupPrivilegeValueA,LookupPrivilegeValueA,1431

	.text.windows
LookupPrivilegeValueA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LookupPrivilegeValueA(%rip),%rax
	jmp	__sysv2nt
	.endfn	LookupPrivilegeValueA,globl
	.previous
