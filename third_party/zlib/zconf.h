#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_ZCONF_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_ZCONF_H_

#define STDC
#define STDC99
#define MAX_MEM_LEVEL 9
#define DEF_MEM_LEVEL 8
#define MAX_WBITS     15 /* 32K LZ77 window */

#ifdef MODE_DBG
#define ZLIB_DEBUG
#endif

#define z_const const

#define Z_COSMO_PREFIX_SET

#define Bytef                    _Cz_Bytef
#define _dist_code               _Cz__dist_code
#define _length_code             _Cz__length_code
#define _tr_align                _Cz__tr_align
#define _tr_flush_bits           _Cz__tr_flush_bits
#define _tr_flush_block          _Cz__tr_flush_block
#define _tr_init                 _Cz__tr_init
#define _tr_stored_block         _Cz__tr_stored_block
#define _tr_tally                _Cz__tr_tally
#define adler32                  _Cz_adler32
#define adler32_combine          _Cz_adler32_combine
#define adler32_combine64        _Cz_adler32_combine64
#define adler32_simd_            _Cz_adler32_simd_
#define adler32_z                _Cz_adler32_z
#define alloc_func               _Cz_alloc_func
#define arm_check_features       _Cz_arm_check_features
#define arm_cpu_enable_crc32     _Cz_arm_cpu_enable_crc32
#define arm_cpu_enable_pmull     _Cz_arm_cpu_enable_pmull
#define armv8_crc32_little       _Cz_armv8_crc32_little
#define armv8_crc32_pmull_little _Cz_armv8_crc32_pmull_little
#define charf                    _Cz_charf
#define compress                 _Cz_compress
#define compress2                _Cz_compress2
#define compressBound            _Cz_compressBound
#define copy_with_crc            _Cz_copy_with_crc
#define cpu_check_features       _Cz_cpu_check_features
#define crc32                    _Cz_crc32
#define crc32_combine            _Cz_crc32_combine
#define crc32_combine64          _Cz_crc32_combine64
#define crc32_combine_gen        _Cz_crc32_combine_gen
#define crc32_combine_gen64      _Cz_crc32_combine_gen64
#define crc32_combine_op         _Cz_crc32_combine_op
#define crc32_sse42_simd_        _Cz_crc32_sse42_simd_
#define crc32_z                  _Cz_crc32_z
#define crc_finalize             _Cz_crc_finalize
#define crc_fold_512to32         _Cz_crc_fold_512to32
#define crc_fold_copy            _Cz_crc_fold_copy
#define crc_fold_init            _Cz_crc_fold_init
#define crc_reset                _Cz_crc_reset
#define deflate                  _Cz_deflate
#define deflateBound             _Cz_deflateBound
#define deflateCopy              _Cz_deflateCopy
#define deflateEnd               _Cz_deflateEnd
#define deflateGetDictionary     _Cz_deflateGetDictionary
#define deflateInit              _Cz_deflateInit
#define deflateInit2             _Cz_deflateInit2
#define deflateParams            _Cz_deflateParams
#define deflatePending           _Cz_deflatePending
#define deflatePrime             _Cz_deflatePrime
#define deflateReset             _Cz_deflateReset
#define deflateResetKeep         _Cz_deflateResetKeep
#define deflateSetDictionary     _Cz_deflateSetDictionary
#define deflateSetHeader         _Cz_deflateSetHeader
#define deflateTune              _Cz_deflateTune
#define deflate_copyright        _Cz_deflate_copyright
#define deflate_read_buf         _Cz_deflate_read_buf
#define fill_window_sse          _Cz_fill_window_sse
#define free_func                _Cz_free_func
#define get_crc_table            _Cz_get_crc_table
#define gzFile                   _Cz_gzFile
#define gz_error                 _Cz_gz_error
#define gz_header                _Cz_gz_header
#define gz_header_s              _Cz_gz_header_s
#define gz_headerp               _Cz_gz_headerp
#define gz_intmax                _Cz_gz_intmax
#define gz_strwinerror           _Cz_gz_strwinerror
#define gzbuffer                 _Cz_gzbuffer
#define gzclearerr               _Cz_gzclearerr
#define gzclose                  _Cz_gzclose
#define gzclose_r                _Cz_gzclose_r
#define gzclose_w                _Cz_gzclose_w
#define gzdirect                 _Cz_gzdirect
#define gzdopen                  _Cz_gzdopen
#define gzeof                    _Cz_gzeof
#define gzerror                  _Cz_gzerror
#define gzflush                  _Cz_gzflush
#define gzfread                  _Cz_gzfread
#define gzfwrite                 _Cz_gzfwrite
#define gzgetc                   _Cz_gzgetc
#define gzgetc_                  _Cz_gzgetc_
#define gzgets                   _Cz_gzgets
#define gzoffset                 _Cz_gzoffset
#define gzoffset64               _Cz_gzoffset64
#define gzopen                   _Cz_gzopen
#define gzopen64                 _Cz_gzopen64
#define gzopen_w                 _Cz_gzopen_w
#define gzprintf                 _Cz_gzprintf
#define gzputc                   _Cz_gzputc
#define gzputs                   _Cz_gzputs
#define gzread                   _Cz_gzread
#define gzrewind                 _Cz_gzrewind
#define gzseek                   _Cz_gzseek
#define gzseek64                 _Cz_gzseek64
#define gzsetparams              _Cz_gzsetparams
#define gztell                   _Cz_gztell
#define gztell64                 _Cz_gztell64
#define gzungetc                 _Cz_gzungetc
#define gzvprintf                _Cz_gzvprintf
#define gzwrite                  _Cz_gzwrite
#define in_func                  _Cz_in_func
#define inflate                  _Cz_inflate
#define inflateBack              _Cz_inflateBack
#define inflateBackEnd           _Cz_inflateBackEnd
#define inflateBackInit_         _Cz_inflateBackInit_
#define inflateCodesUsed         _Cz_inflateCodesUsed
#define inflateCopy              _Cz_inflateCopy
#define inflateEnd               _Cz_inflateEnd
#define inflateGetDictionary     _Cz_inflateGetDictionary
#define inflateGetHeader         _Cz_inflateGetHeader
#define inflateInit              _Cz_inflateInit
#define inflateInit2             _Cz_inflateInit2
#define inflateMark              _Cz_inflateMark
#define inflatePrime             _Cz_inflatePrime
#define inflateReset             _Cz_inflateReset
#define inflateReset2            _Cz_inflateReset2
#define inflateResetKeep         _Cz_inflateResetKeep
#define inflateSetDictionary     _Cz_inflateSetDictionary
#define inflateSync              _Cz_inflateSync
#define inflateSyncPoint         _Cz_inflateSyncPoint
#define inflateUndermine         _Cz_inflateUndermine
#define inflateValidate          _Cz_inflateValidate
#define inflate_copyright        _Cz_inflate_copyright
#define inflate_fast             _Cz_inflate_fast
#define inflate_fast_chunk_      _Cz_inflate_fast_chunk_
#define inflate_table            _Cz_inflate_table
#define intf                     _Cz_intf
#define out_func                 _Cz_out_func
#define uInt                     _Cz_uInt
#define uIntf                    _Cz_uIntf
#define uLong                    _Cz_uLong
#define uLongf                   _Cz_uLongf
#define uncompress               _Cz_uncompress
#define uncompress2              _Cz_uncompress2
#define voidp                    _Cz_voidp
#define voidpc                   _Cz_voidpc
#define voidpf                   _Cz_voidpf
#define x86_check_features       _Cz_x86_check_features
#define x86_cpu_enable_simd      _Cz_x86_cpu_enable_simd
#define x86_cpu_enable_sse2      _Cz_x86_cpu_enable_sse2
#define x86_cpu_enable_ssse3     _Cz_x86_cpu_enable_ssse3
#define zError                   _Cz_zError
#define z_errmsg                 _Cz_z_errmsg
#define zcalloc                  _Cz_zcalloc
#define zcfree                   _Cz_zcfree
#define zlibCompileFlags         _Cz_zlibCompileFlags
#define zlibVersion              _Cz_zlibVersion


typedef unsigned char Byte;
typedef unsigned int uInt;   /* 16 bits or more */
typedef unsigned long uLong; /* 32 bits or more */
typedef Byte Bytef;
typedef char charf;
typedef int intf;
typedef uInt uIntf;
typedef uLong uLongf;
typedef void const *voidpc;
typedef void *voidpf;
typedef void *voidp;
typedef uint32_t z_crc_t;
typedef int64_t z_off64_t;
typedef size_t z_size_t;

#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_ZCONF_H_ */
