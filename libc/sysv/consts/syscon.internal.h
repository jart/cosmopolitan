#include "libc/dce.h"
#include "libc/macros.internal.h"

.macro	.syscon	group:req name:req linux:req xnu:req freebsd:req openbsd:req netbsd:req windows:req
	yoink	_init_systemfive

	.section .piro.bss.sort.syscon.2.\group\().\name,"aw",@nobits
\name:	.quad	0
	.endobj	\name,globl
	.previous

#if SupportsLinux() || SupportsMetal()
	.section .sort.rodata.syscon.linux.2.\group\().\name,"a",@progbits
	.uleb128 \linux
	.previous
#endif

#if SupportsXnu()
	.section .sort.rodata.syscon.xnu.2.\group\().\name,"a",@progbits
	.uleb128 \xnu
	.previous
#endif

#if SupportsFreebsd()
	.section .sort.rodata.syscon.freebsd.2.\group\().\name,"a",@progbits
	.uleb128 \freebsd
	.previous
#endif

#if SupportsOpenbsd()
	.section .sort.rodata.syscon.openbsd.2.\group\().\name,"a",@progbits
	.uleb128 \openbsd
	.previous
#endif

#if SupportsNetbsd()
	.section .sort.rodata.syscon.netbsd.2.\group\().\name,"a",@progbits
	.uleb128 \netbsd
	.previous
#endif

#if SupportsWindows()
	.section .sort.rodata.syscon.windows.2.\group\().\name,"a",@progbits
	.uleb128 \windows
	.previous
#endif

.endm
