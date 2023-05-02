#include "libc/nt/codegen.h"
.imp	advapi32,__imp_LookupPrivilegeValueW,LookupPrivilegeValueW,1432

#ifdef __x86_64__
	.text.windows
LookupPrivilegeValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LookupPrivilegeValueW(%rip),%rax
	jmp	__sysv2nt
	.endfn	LookupPrivilegeValue,globl
	.previous
#endif
