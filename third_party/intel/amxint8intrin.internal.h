#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <amxint8intrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AMXINT8INTRIN_H_INCLUDED
#define _AMXINT8INTRIN_H_INCLUDED
#if !defined(__AMX_INT8__)
#pragma GCC push_options
#pragma GCC target("amx-int8")
#define __DISABLE_AMX_INT8__
#endif
#if defined(__x86_64__) && defined(__AMX_INT8__)
#define _tile_int8_dp_internal(name,dst,src1,src2) __asm__ volatile ("{"#name"\t%%tmm"#src2", %%tmm"#src1", %%tmm"#dst"|"#name"\t%%tmm"#dst", %%tmm"#src1", %%tmm"#src2"}" ::)
#define _tile_dpbssd(dst,src1,src2) _tile_int8_dp_internal (tdpbssd, dst, src1, src2)
#define _tile_dpbsud(dst,src1,src2) _tile_int8_dp_internal (tdpbsud, dst, src1, src2)
#define _tile_dpbusd(dst,src1,src2) _tile_int8_dp_internal (tdpbusd, dst, src1, src2)
#define _tile_dpbuud(dst,src1,src2) _tile_int8_dp_internal (tdpbuud, dst, src1, src2)
#endif
#ifdef __DISABLE_AMX_INT8__
#undef __DISABLE_AMX_INT8__
#pragma GCC pop_options
#endif
#endif
#endif
