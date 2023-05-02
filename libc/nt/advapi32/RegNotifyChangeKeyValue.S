#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegNotifyChangeKeyValue,RegNotifyChangeKeyValue,0

#ifdef __x86_64__
	.text.windows
RegNotifyChangeKeyValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegNotifyChangeKeyValue(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegNotifyChangeKeyValue,globl
	.previous
#endif
