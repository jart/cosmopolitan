#ifndef COSMOPOLITAN_LIBC_ZIP_H_
#define COSMOPOLITAN_LIBC_ZIP_H_
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/bits.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

/**
 * @fileoverview PKZIP Data Structures.
 */

#ifdef __ASSEMBLER__
#define ZM_(x) x
#else
#define ZM_(x) ~VEIL("r", ~x) /* prevent magic from appearing in binary */
#endif

#define kZipOk                       0
#define kZipErrorEocdNotFound        -1
#define kZipErrorEocdOffsetOverflow  -2
#define kZipErrorEocdMagicNotFound   -3
#define kZipErrorEocdSizeOverflow    -4
#define kZipErrorEocdDiskMismatch    -5
#define kZipErrorCdirRecordsMismatch -6
#define kZipErrorCdirRecordsOverflow -7
#define kZipErrorCdirOffsetPastEocd  -8
#define kZipErrorCdirLocatorMagic    -9
#define kZipErrorCdirLocatorOffset   -10
#define kZipErrorRaceCondition       -11
#define kZipErrorMapFailed           -12
#define kZipErrorOpenFailed          -13

#define kZipCosmopolitanVersion kZipEra2001

#define kZipOsDos         0
#define kZipOsAmiga       1
#define kZipOsOpenvms     2
#define kZipOsUnix        3
#define kZipOsVmcms       4
#define kZipOsAtarist     5
#define kZipOsOs2hpfs     6
#define kZipOsMacintosh   7
#define kZipOsZsystem     8
#define kZipOsCpm         9
#define kZipOsWindowsntfs 10
#define kZipOsMvsos390zos 11
#define kZipOsVse         12
#define kZipOsAcornrisc   13
#define kZipOsVfat        14
#define kZipOsAltmvs      15
#define kZipOsBeos        16
#define kZipOsTandem      17
#define kZipOsOs400       18
#define kZipOsOsxdarwin   19

#define kZipEra1989 10 /* PKZIP 1.0 */
#define kZipEra1993 20 /* PKZIP 2.0: deflate/subdir/etc. support */
#define kZipEra2001 45 /* PKZIP 4.5: kZipExtraZip64 support */

#define kZipIattrBinary 0 /* first bit not set */
#define kZipIattrText   1 /* first bit set */

#define kZipCompressionNone    0
#define kZipCompressionDeflate 8

#define kZipCdirHdrMagic            ZM_(0x06054b50) /* PK♣♠ "PK\5\6" */
#define kZipCdirHdrMagicTodo        ZM_(0x19184b50) /* PK♣♠ "PK\30\31" */
#define kZipCdirHdrMinSize          22
#define kZipCdirHdrLinkableSize     294
#define kZipCdirDiskOffset          4
#define kZipCdirStartingDiskOffset  6
#define kZipCdirRecordsOnDiskOffset 8
#define kZipCdirRecordsOffset       10
#define kZipCdirSizeOffset          12
#define kZipCdirOffsetOffset        16
#define kZipCdirCommentSizeOffset   20

#define kZipCdir64HdrMagic     ZM_(0x06064b50) /* PK♣♠ "PK\6\6" */
#define kZipCdir64HdrMinSize   56
#define kZipCdir64LocatorMagic ZM_(0x07064b50) /* PK♠• "PK\6\7" */
#define kZipCdir64LocatorSize  20

#define kZipCfileHdrMagic                 ZM_(0x02014b50) /* PK☺☻ "PK\1\2" */
#define kZipCfileHdrMinSize               46
#define kZipCfileOffsetGeneralflag        8
#define kZipCfileOffsetCompressionmethod  10
#define kZipCfileOffsetLastmodifiedtime   12
#define kZipCfileOffsetLastmodifieddate   14
#define kZipCfileOffsetCrc32              16
#define kZipCfileOffsetCompressedsize     20
#define kZipCfileOffsetUncompressedsize   24
#define kZipCfileOffsetExternalattributes 38
#define kZipCfileOffsetOffset             42

#define kZipLfileHdrMagic                ZM_(0x04034b50) /* PK♥♦ "PK\3\4" */
#define kZipLfileHdrMinSize              30
#define kZipLfileOffsetGeneralflag       6
#define kZipLfileOffsetCompressionmethod 8
#define kZipLfileOffsetLastmodifiedtime  10
#define kZipLfileOffsetLastmodifieddate  12
#define kZipLfileOffsetCrc32             14
#define kZipLfileOffsetCompressedsize    18
#define kZipLfileOffsetUncompressedsize  22

#define kZipGflagUtf8 0x800

#define kZipExtraHdrSize             4
#define kZipExtraZip64               0x0001
#define kZipExtraNtfs                0x000a
#define kZipExtraUnix                0x000d
#define kZipExtraExtendedTimestamp   0x5455
#define kZipExtraInfoZipNewUnixExtra 0x7875

#define kZipCfileMagic "PK\001\002"

#if !(__ASSEMBLER__ + __LINKER__ + 0)

/* end of central directory record */
#define ZIP_CDIR_MAGIC(P)         READ32LE(P)
#define ZIP_CDIR_DISK(P)          READ16LE((P) + kZipCdirDiskOffset)
#define ZIP_CDIR_STARTINGDISK(P)  READ16LE((P) + kZipCdirStartingDiskOffset)
#define ZIP_CDIR_RECORDSONDISK(P) READ16LE((P) + kZipCdirRecordsOnDiskOffset)
#define ZIP_CDIR_RECORDS(P)       READ16LE((P) + kZipCdirRecordsOffset)
#define ZIP_CDIR_SIZE(P)          READ32LE((P) + kZipCdirSizeOffset)
#define ZIP_CDIR_OFFSET(P)        READ32LE((P) + kZipCdirOffsetOffset)
#define ZIP_CDIR_COMMENTSIZE(P)   READ16LE((P) + kZipCdirCommentSizeOffset)
#define ZIP_CDIR_COMMENT(P)       ((P) + 22) /* recommend stopping at nul */
#define ZIP_CDIR_HDRSIZE(P)       (ZIP_CDIR_COMMENTSIZE(P) + kZipCdirHdrMinSize)

/* zip64 end of central directory record */
#define ZIP_CDIR64_MAGIC(P)         READ32LE(P)
#define ZIP_CDIR64_HDRSIZE(P)       (READ64LE((P) + 4) + 12)
#define ZIP_CDIR64_VERSIONMADE(P)   READ16LE((P) + 12)
#define ZIP_CDIR64_VERSIONNEED(P)   READ16LE((P) + 14)
#define ZIP_CDIR64_DISK(P)          READ32LE((P) + 16)
#define ZIP_CDIR64_STARTINGDISK(P)  READ32LE((P) + 20)
#define ZIP_CDIR64_RECORDSONDISK(P) READ64LE((P) + 24)
#define ZIP_CDIR64_RECORDS(P)       READ64LE((P) + 32)
#define ZIP_CDIR64_SIZE(P)          READ64LE((P) + 40)
#define ZIP_CDIR64_OFFSET(P)        READ64LE((P) + 48)
#define ZIP_CDIR64_COMMENTSIZE(P) \
  (ZIP_CDIR64_HDRSIZE(P) >= 56 ? ZIP_CDIR64_HDRSIZE(P) - 56 : 0)
#define ZIP_CDIR64_COMMENT(P)        ((P) + 56) /* recommend stopping at nul */
#define ZIP_LOCATE64_MAGIC(P)        READ32LE(P)
#define ZIP_LOCATE64_STARTINGDISK(P) READ32LE((P) + 4)
#define ZIP_LOCATE64_OFFSET(P)       READ64LE((P) + 8)
#define ZIP_LOCATE64_TOTALDISKS(P)   READ32LE((P) + 12)

/* central directory file header */
#define ZIP_CFILE_MAGIC(P)          READ32LE(P)
#define ZIP_CFILE_VERSIONMADE(P)    (255 & (P)[4])
#define ZIP_CFILE_FILEATTRCOMPAT(P) (255 & (P)[5])
#define ZIP_CFILE_VERSIONNEED(P)    (255 & (P)[6])
#define ZIP_CFILE_OSNEED(P)         (255 & (P)[7])
#define ZIP_CFILE_GENERALFLAG(P)    READ16LE((P) + kZipCfileOffsetGeneralflag)
#define ZIP_CFILE_COMPRESSIONMETHOD(P) \
  READ16LE((P) + kZipCfileOffsetCompressionmethod)
#define ZIP_CFILE_LASTMODIFIEDTIME(P) \
  READ16LE((P) + kZipCfileOffsetLastmodifiedtime) /* @see DOS_TIME() */
#define ZIP_CFILE_LASTMODIFIEDDATE(P) \
  READ16LE((P) + kZipCfileOffsetLastmodifieddate) /* @see DOS_DATE() */
#define ZIP_CFILE_CRC32(P)          READ32LE((P) + kZipCfileOffsetCrc32)
#define ZIP_CFILE_COMPRESSEDSIZE(P) READ32LE(P + kZipCfileOffsetCompressedsize)
#define ZIP_CFILE_UNCOMPRESSEDSIZE(P) \
  READ32LE((P) + kZipCfileOffsetUncompressedsize)
#define ZIP_CFILE_NAMESIZE(P)           READ16LE((P) + 28)
#define ZIP_CFILE_EXTRASIZE(P)          READ16LE((P) + 30)
#define ZIP_CFILE_COMMENTSIZE(P)        READ16LE((P) + 32)
#define ZIP_CFILE_DISK(P)               READ16LE((P) + 34)
#define ZIP_CFILE_INTERNALATTRIBUTES(P) READ16LE((P) + 36)
#define ZIP_CFILE_EXTERNALATTRIBUTES(P) \
  READ32LE((P) + kZipCfileOffsetExternalattributes)
#define ZIP_CFILE_OFFSET(P) READ32LE((P) + kZipCfileOffsetOffset)
#define ZIP_CFILE_NAME(P)   ((const char *)((P) + 46)) /* not nul-terminated */
#define ZIP_CFILE_EXTRA(P)  ((P) + 46 + ZIP_CFILE_NAMESIZE(P))
#define ZIP_CFILE_COMMENT(P)                         \
  ((const char *)((P) + 46 + ZIP_CFILE_NAMESIZE(P) + \
                  ZIP_CFILE_EXTRASIZE(P))) /* recommend stopping at nul */
#define ZIP_CFILE_HDRSIZE(P)                                                   \
  (ZIP_CFILE_NAMESIZE(P) + ZIP_CFILE_EXTRASIZE(P) + ZIP_CFILE_COMMENTSIZE(P) + \
   kZipCfileHdrMinSize)

/* local file header */
#define ZIP_LFILE_MAGIC(P)       READ32LE(P)
#define ZIP_LFILE_VERSIONNEED(P) (255 & (P)[4])
#define ZIP_LFILE_OSNEED(P)      (255 & (P)[5])
#define ZIP_LFILE_GENERALFLAG(P) READ16LE((P) + kZipLfileOffsetGeneralflag)
#define ZIP_LFILE_COMPRESSIONMETHOD(P) \
  READ16LE((P) + kZipLfileOffsetCompressionmethod)
#define ZIP_LFILE_LASTMODIFIEDTIME(P) \
  READ16LE((P) + kZipLfileOffsetLastmodifiedtime) /* @see DOS_TIME() */
#define ZIP_LFILE_LASTMODIFIEDDATE(P) \
  READ16LE((P) + kZipLfileOffsetLastmodifieddate) /* @see DOS_DATE() */
#define ZIP_LFILE_CRC32(P) READ32LE((P) + kZipLfileOffsetCrc32)
#define ZIP_LFILE_COMPRESSEDSIZE(P) \
  READ32LE((P) + kZipLfileOffsetCompressedsize)
#define ZIP_LFILE_UNCOMPRESSEDSIZE(P) \
  READ32LE((P) + kZipLfileOffsetUncompressedsize)
#define ZIP_LFILE_NAMESIZE(P)  READ16LE((P) + 26)
#define ZIP_LFILE_EXTRASIZE(P) READ16LE((P) + 28)
#define ZIP_LFILE_NAME(P)      ((const char *)((P) + 30))
#define ZIP_LFILE_EXTRA(P)     ((P) + 30 + ZIP_LFILE_NAMESIZE(P))
#define ZIP_LFILE_HDRSIZE(P) \
  (ZIP_LFILE_NAMESIZE(P) + ZIP_LFILE_EXTRASIZE(P) + kZipLfileHdrMinSize)
#define ZIP_LFILE_CONTENT(P) ((P) + ZIP_LFILE_HDRSIZE(P))
#define ZIP_LFILE_SIZE(P)    (ZIP_LFILE_HDRSIZE(P) + ZIP_LFILE_COMPRESSEDSIZE(P))

#define ZIP_EXTRA_HEADERID(P)    READ16LE(P)
#define ZIP_EXTRA_CONTENTSIZE(P) READ16LE((P) + 2)
#define ZIP_EXTRA_CONTENT(P)     ((P) + 4)
#define ZIP_EXTRA_SIZE(P)        (ZIP_EXTRA_CONTENTSIZE(P) + kZipExtraHdrSize)

void *GetZipEocd(const void *, size_t, int *);
uint8_t *FindEmbeddedApe(const uint8_t *, size_t);
int IsZipEocd32(const uint8_t *, size_t, size_t);
int IsZipEocd64(const uint8_t *, size_t, size_t);
int GetZipCfileMode(const uint8_t *);
uint64_t GetZipCdirOffset(const uint8_t *);
uint64_t GetZipCdirRecords(const uint8_t *);
void *GetZipCdirComment(const uint8_t *);
uint64_t GetZipCdirSize(const uint8_t *);
uint64_t GetZipCdirCommentSize(const uint8_t *);
uint64_t GetZipCfileUncompressedSize(const uint8_t *);
uint64_t GetZipCfileCompressedSize(const uint8_t *);
uint64_t GetZipCfileOffset(const uint8_t *);
uint64_t GetZipLfileUncompressedSize(const uint8_t *);
uint64_t GetZipLfileCompressedSize(const uint8_t *);
void GetZipCfileTimestamps(const uint8_t *, struct timespec *,
                           struct timespec *, struct timespec *, int);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ZIP_H_ */