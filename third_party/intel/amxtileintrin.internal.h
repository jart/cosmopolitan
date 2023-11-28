#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
#error "Never use <amxtileintrin.h> directly; include <immintrin.h> instead."
#endif
#ifndef _AMXTILEINTRIN_H_INCLUDED
#define _AMXTILEINTRIN_H_INCLUDED
#if !defined(__AMX_TILE__)
#pragma GCC push_options
#pragma GCC target("amx-tile")
#define __DISABLE_AMX_TILE__
#endif
#if defined(__x86_64__) && defined(__AMX_TILE__)
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_tile_loadconfig (const void *__config)
{
  __asm__ volatile ("ldtilecfg\t%X0" :: "m" (*((const void **)__config)));
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_tile_storeconfig (void *__config)
{
  __asm__ volatile ("sttilecfg\t%X0" : "=m" (*((void **)__config)));
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_tile_release (void)
{
  __asm__ volatile ("tilerelease" ::);
}
#define _tile_loadd(dst,base,stride) _tile_loadd_internal (dst, base, stride)
#define _tile_loadd_internal(dst,base,stride) __asm__ volatile ("{tileloadd\t(%0,%1,1), %%tmm"#dst"|tileloadd\t%%tmm"#dst", [%0+%1*1]}" :: "r" ((const void*) base), "r" ((long) stride))
#define _tile_stream_loadd(dst,base,stride) _tile_stream_loadd_internal (dst, base, stride)
#define _tile_stream_loadd_internal(dst,base,stride) __asm__ volatile ("{tileloaddt1\t(%0,%1,1), %%tmm"#dst"|tileloaddt1\t%%tmm"#dst", [%0+%1*1]}" :: "r" ((const void*) base), "r" ((long) stride))
#define _tile_stored(dst,base,stride) _tile_stored_internal (dst, base, stride)
#define _tile_stored_internal(src,base,stride) __asm__ volatile ("{tilestored\t%%tmm"#src", (%0,%1,1)|tilestored\t[%0+%1*1], %%tmm"#src"}" :: "r" ((void*) base), "r" ((long) stride) : "memory")
#define _tile_zero(dst) _tile_zero_internal (dst)
#define _tile_zero_internal(dst) __asm__ volatile ("tilezero\t%%tmm"#dst ::)
#endif
#ifdef __DISABLE_AMX_TILE__
#undef __DISABLE_AMX_TILE__
#pragma GCC pop_options
#endif
#endif
#endif
