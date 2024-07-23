#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <amxcomplexintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AMXCOMPLEXINTRIN_H_INCLUDED
#define _AMXCOMPLEXINTRIN_H_INCLUDED
#if !defined(__AMX_COMPLEX__)
#pragma GCC push_options
#pragma GCC target("amx-complex")
#define __DISABLE_AMX_COMPLEX__
#endif
#if defined(__x86_64__)
#define _tile_cmmimfp16ps_internal(src1_dst,src2,src3) __asm__ volatile ("{tcmmimfp16ps\t%%tmm"#src3", %%tmm"#src2", %%tmm"#src1_dst"|tcmmimfp16ps\t%%tmm"#src1_dst", %%tmm"#src2", %%tmm"#src3"}" ::)
#define _tile_cmmrlfp16ps_internal(src1_dst,src2,src3) __asm__ volatile ("{tcmmrlfp16ps\t%%tmm"#src3", %%tmm"#src2", %%tmm"#src1_dst"|tcmmrlfp16ps\t%%tmm"#src1_dst", %%tmm"#src2", %%tmm"#src3"}" ::)
#define _tile_cmmimfp16ps(src1_dst,src2,src3) _tile_cmmimfp16ps_internal (src1_dst, src2, src3)
#define _tile_cmmrlfp16ps(src1_dst,src2,src3) _tile_cmmrlfp16ps_internal (src1_dst, src2, src3)
#endif
#ifdef __DISABLE_AMX_COMPLEX__
#undef __DISABLE_AMX_COMPLEX__
#pragma GCC pop_options
#endif
#endif
#endif
