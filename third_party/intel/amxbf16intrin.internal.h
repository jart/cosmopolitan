#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <amxbf16intrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AMXBF16INTRIN_H_INCLUDED
#define _AMXBF16INTRIN_H_INCLUDED
#if !defined(__AMX_BF16__)
#pragma GCC push_options
#pragma GCC target("amx-bf16")
#define __DISABLE_AMX_BF16__
#endif
#if defined(__x86_64__) && defined(__AMX_BF16__)
#define _tile_dpbf16ps_internal(dst,src1,src2) __asm__ volatile ("{tdpbf16ps\t%%tmm"#src2", %%tmm"#src1", %%tmm"#dst"|tdpbf16ps\t%%tmm"#dst", %%tmm"#src1", %%tmm"#src2"}" ::)
#define _tile_dpbf16ps(dst,src1,src2) _tile_dpbf16ps_internal (dst, src1, src2)
#endif
#ifdef __DISABLE_AMX_BF16__
#undef __DISABLE_AMX_BF16__
#pragma GCC pop_options
#endif
#endif
#endif
