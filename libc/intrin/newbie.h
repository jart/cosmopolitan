#ifndef COSMOPOLITAN_LIBC_BITS_NEWBIE_H_
#define COSMOPOLITAN_LIBC_BITS_NEWBIE_H_
#include "libc/intrin/bswap.h"

#define BYTE_ORDER    __BYTE_ORDER__
#define LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#define BIG_ENDIAN    __ORDER_BIG_ENDIAN__
#define PDP_ENDIAN    __ORDER_PDP_ENDIAN__

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define htobe16(x) bswap_16(x)
#define be16toh(x) bswap_16(x)
#define betoh16(x) bswap_16(x)
#define htobe32(x) bswap_32(x)
#define be32toh(x) bswap_32(x)
#define betoh32(x) bswap_32(x)
#define htobe64(x) bswap_64(x)
#define be64toh(x) bswap_64(x)
#define betoh64(x) bswap_64(x)
#define htole16(x) (uint16_t)(x)
#define le16toh(x) (uint16_t)(x)
#define letoh16(x) (uint16_t)(x)
#define htole32(x) (uint32_t)(x)
#define le32toh(x) (uint32_t)(x)
#define letoh32(x) (uint32_t)(x)
#define htole64(x) (uint64_t)(x)
#define le64toh(x) (uint64_t)(x)
#define letoh64(x) (uint64_t)(x)
#else
#define htobe16(x) (uint16_t)(x)
#define be16toh(x) (uint16_t)(x)
#define betoh16(x) (uint16_t)(x)
#define htobe32(x) (uint32_t)(x)
#define be32toh(x) (uint32_t)(x)
#define betoh32(x) (uint32_t)(x)
#define htobe64(x) (uint64_t)(x)
#define be64toh(x) (uint64_t)(x)
#define betoh64(x) (uint64_t)(x)
#define htole16(x) bswap_16(x)
#define le16toh(x) bswap_16(x)
#define letoh16(x) bswap_16(x)
#define htole32(x) bswap_32(x)
#define le32toh(x) bswap_32(x)
#define letoh32(x) bswap_32(x)
#define htole64(x) bswap_64(x)
#define le64toh(x) bswap_64(x)
#define letoh64(x) bswap_64(x)
#endif

#endif /* COSMOPOLITAN_LIBC_BITS_NEWBIE_H_ */
