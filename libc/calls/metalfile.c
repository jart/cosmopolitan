/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/relocations.h"
#include "ape/sections.internal.h"
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/metalfile.internal.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/errfuns.h"

#ifdef __x86_64__

#define MAP_ANONYMOUS_linux 0x00000020
#define MAP_FIXED_linux     0x00000010
#define MAP_SHARED_linux    0x00000001

__static_yoink("_init_metalfile");

void *__ape_com_base;
size_t __ape_com_size = 0;

textstartup void InitializeMetalFile(void) {
  if (IsMetal()) {
    /*
     * Copy out a pristine image of the program — before the program might
     * decide to modify its own .data section.
     *
     * This code is included if a symbol "file:/proc/self/exe" is defined
     * (see libc/calls/metalfile.internal.h & libc/calls/metalfile_init.S).
     * The zipos code will automatically arrange to do this.  Alternatively,
     * user code can __static_yoink this symbol.
     */
    size_t size = ROUNDUP(_ezip - __executable_start, 4096);
    // TODO(jart): Restore support for ZIPOS on metal.
    void *copied_base;
    struct DirectMap dm;
    dm = sys_mmap_metal(NULL, size, PROT_READ | PROT_WRITE,
                        MAP_SHARED_linux | MAP_ANONYMOUS_linux, -1, 0);
    copied_base = dm.addr;
    npassert(copied_base != (void *)-1);
    memcpy(copied_base, (void *)(BANE + IMAGE_BASE_PHYSICAL), size);
    __ape_com_base = copied_base;
    __ape_com_size = size;
    // TODO(tkchia): LIBC_CALLS doesn't depend on LIBC_VGA so references
    //               to its functions need to be weak
    // KINFOF("%s @ %p,+%#zx", APE_COM_NAME, copied_base, size);
  }
}

#endif /* __x86_64__ */
