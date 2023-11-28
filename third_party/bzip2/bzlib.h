#ifndef _BZLIB_H
#define _BZLIB_H
#include "libc/stdio/stdio.h"

#define BZ_RUN    0
#define BZ_FLUSH  1
#define BZ_FINISH 2

#define BZ_OK               0
#define BZ_RUN_OK           1
#define BZ_FLUSH_OK         2
#define BZ_FINISH_OK        3
#define BZ_STREAM_END       4
#define BZ_SEQUENCE_ERROR   (-1)
#define BZ_PARAM_ERROR      (-2)
#define BZ_MEM_ERROR        (-3)
#define BZ_DATA_ERROR       (-4)
#define BZ_DATA_ERROR_MAGIC (-5)
#define BZ_IO_ERROR         (-6)
#define BZ_UNEXPECTED_EOF   (-7)
#define BZ_OUTBUFF_FULL     (-8)
#define BZ_CONFIG_ERROR     (-9)

#define BZ_MAX_UNUSED 5000

COSMOPOLITAN_C_START_

typedef struct {
  char *next_in;
  unsigned int avail_in;
  unsigned int total_in_lo32;
  unsigned int total_in_hi32;
  char *next_out;
  unsigned int avail_out;
  unsigned int total_out_lo32;
  unsigned int total_out_hi32;
  void *state;
  void *(*bzalloc)(void *, int, int);
  void (*bzfree)(void *, void *);
  void *opaque;
} bz_stream;

/*-- Core (low-level) library functions --*/

int BZ2_bzCompressInit(bz_stream *, int, int, int);
int BZ2_bzCompress(bz_stream *, int);
int BZ2_bzCompressEnd(bz_stream *);
int BZ2_bzDecompressInit(bz_stream *, int, int);
int BZ2_bzDecompress(bz_stream *);
int BZ2_bzDecompressEnd(bz_stream *);

/*-- High(er) level library functions --*/

typedef void BZFILE;

BZFILE *BZ2_bzReadOpen(int *, FILE *, int, int, void *, int);
void BZ2_bzReadClose(int *, BZFILE *);
void BZ2_bzReadGetUnused(int *, BZFILE *, void **, int *);
int BZ2_bzRead(int *, BZFILE *, void *, int);
BZFILE *BZ2_bzWriteOpen(int *, FILE *, int, int, int);
void BZ2_bzWrite(int *, BZFILE *, void *, int);
void BZ2_bzWriteClose(int *, BZFILE *, int, unsigned int *, unsigned int *);
void BZ2_bzWriteClose64(int *, BZFILE *, int, unsigned int *, unsigned int *,
                        unsigned int *, unsigned int *);
int BZ2_bzBuffToBuffCompress(char *, unsigned int *, char *, unsigned int, int,
                             int, int);
int BZ2_bzBuffToBuffDecompress(char *, unsigned int *, char *, unsigned int,
                               int, int);

/*--
   Code contributed by Yoshioka Tsuneo (tsuneo@rr.iij4u.or.jp)
   to support better zlib compatibility.
   This code is not _officially_ part of libbzip2 (yet);
   I haven't tested it, documented it, or considered the
   threading-safeness of it.
   If this code breaks, please contact both Yoshioka and me.
--*/

const char *BZ2_bzlibVersion(void);
BZFILE *BZ2_bzopen(const char *, const char *);
BZFILE *BZ2_bzdopen(int, const char *);
int BZ2_bzread(BZFILE *, void *, int);
int BZ2_bzwrite(BZFILE *, void *, int);
int BZ2_bzflush(BZFILE *);
void BZ2_bzclose(BZFILE *);
const char *BZ2_bzerror(BZFILE *, int *);

COSMOPOLITAN_C_END_
#endif /* _BZLIB_H */
