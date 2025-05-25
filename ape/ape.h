#ifndef COSMOPOLITAN_APE_APE_H_
#define COSMOPOLITAN_APE_APE_H_

#define APE_VERSION_MAJOR 1
#define APE_VERSION_MINOR 10
#define APE_VERSION_STR   APE_VERSION_STR_(APE_VERSION_MAJOR, APE_VERSION_MINOR)
#define APE_VERSION_NOTE  APE_VERSION_NOTE_(APE_VERSION_MAJOR, APE_VERSION_MINOR)

#define APE_VERSION_STR__(x, y) #x "." #y
#define APE_VERSION_STR_(x, y)  APE_VERSION_STR__(x, y)
#define APE_VERSION_NOTE_(x, y) (100000000 * (x) + 1000000 * (y))

#if defined(__x86_64__)
#define GetXnuSuffix() "-x86_64"
#elif defined(__aarch64__)
#define GetXnuSuffix() "-arm64"
#else
#define GetXnuSuffix() "-unknown"
#endif

#endif /* COSMOPOLITAN_APE_APE_H_ */
