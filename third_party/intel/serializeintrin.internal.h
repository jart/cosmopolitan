#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <serializeintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _SERIALIZE_H_INCLUDED
#define _SERIALIZE_H_INCLUDED
#ifndef __SERIALIZE__
#pragma GCC push_options
#pragma GCC target("serialize")
#define __DISABLE_SERIALIZE__
#endif
#define _serialize() __builtin_ia32_serialize ()
#ifdef __DISABLE_SERIALIZE__
#undef __DISABLE_SERIALIZE__
#pragma GCC pop_options
#endif
#endif
#endif
