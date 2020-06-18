#include "libc/macros.h"

/	Example assembly function.
/
/	@note	param agnostic
/	@note	we love stack frames
/		easiest way to do backtraces
/		somehow they usually make code faster
/		it's convention for keeping stack 16-byte aligned
/		cpus still devote much to pushing & popping b/c i386
MyAsm:	push	%rbp
	mov	%rsp,%rbp
	call	MyPrint2
	pop	%rbp
	ret
	.endfn	MyAsm,globl
