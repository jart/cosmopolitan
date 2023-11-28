#ifndef COSMOPOLITAN_LIBC_ISYSTEM_LINUX_TYPES_H_
#define COSMOPOLITAN_LIBC_ISYSTEM_LINUX_TYPES_H_
COSMOPOLITAN_C_START_

typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64;
typedef __u16 __sum16;
typedef __u32 __wsum;

#define __aligned_u64  __u64 __attribute__((__aligned__(8)))
#define __aligned_be64 __be64 __attribute__((__aligned__(8)))
#define __aligned_le64 __le64 __attribute__((__aligned__(8)))

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_ISYSTEM_LINUX_TYPES_H_ */
