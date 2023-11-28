#ifndef COSMOPOLITAN_THIRD_PARTY_NSYNC_RACES_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_NSYNC_RACES_INTERNAL_H_
COSMOPOLITAN_C_START_

/* Annotations for race detectors. */
#if defined(__has_feature) && !defined(__SANITIZE_THREAD__)
#if __has_feature(thread_sanitizer) /* used by clang */
#define __SANITIZE_THREAD__ 1 /* GCC uses this; fake it in clang */
#endif
#endif
#if defined(__SANITIZE_THREAD__)
NSYNC_C_START_
void AnnotateIgnoreWritesBegin(const char *file, int line);
void AnnotateIgnoreWritesEnd(const char *file, int line);
void AnnotateIgnoreReadsBegin(const char *file, int line);
void AnnotateIgnoreReadsEnd(const char *file, int line);
NSYNC_C_END_
#define IGNORE_RACES_START()                       \
  do {                                             \
    AnnotateIgnoreReadsBegin(__FILE__, __LINE__);  \
    AnnotateIgnoreWritesBegin(__FILE__, __LINE__); \
  } while (0)
#define IGNORE_RACES_END()                       \
  do {                                           \
    AnnotateIgnoreWritesEnd(__FILE__, __LINE__); \
    AnnotateIgnoreReadsEnd(__FILE__, __LINE__);  \
  } while (0)
#else
#define IGNORE_RACES_START()
#define IGNORE_RACES_END()
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_NSYNC_RACES_INTERNAL_H_ */
