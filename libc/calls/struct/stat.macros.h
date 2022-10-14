#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_MACROS_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_MACROS_H_

#define STAT_HAVE_NSEC 1

#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec

#define st_atime_nsec st_atim.tv_nsec
#define st_mtime_nsec st_mtim.tv_nsec
#define st_ctime_nsec st_ctim.tv_nsec

#define st_atimensec st_atim.tv_nsec
#define st_mtimensec st_mtim.tv_nsec
#define st_ctimensec st_ctim.tv_nsec
#define st_birthtime st_birthtim.tv_sec

#define st_file_attributes st_flags

#define INIT_STRUCT_STAT_PADDING(st) (void)st

#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_STAT_MACROS_H_ */
