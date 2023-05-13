#include "libc/macros.internal.h"

//	Example assembly function.
//
//	@note	param agnostic
//	@note	we love stack frames
//		easiest way to do backtraces
//		somehow they usually make code faster
//		it's convention for keeping stack 16-byte aligned
//		cpus still devote much to pushing & popping b/c i386
MyAsm:

#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	call	MyPrint2
	pop	%rbp
#elif defined(__aarch64__)
	bl	MyPrint2
#else
#error "unsupported architecture"
#endif

	ret
	.endfn	MyAsm,globl
