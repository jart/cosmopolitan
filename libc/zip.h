#ifndef COSMOPOLITAN_LIBC_ZIP_H_
#define COSMOPOLITAN_LIBC_ZIP_H_
#include "libc/bits/bits.h"
#include "libc/macros.h"
#include "libc/str/str.h"

/**
 * @fileoverview PKZIP Data Structures.
 */

#define kZipAlign 2

#define kZipCosmopolitanVersion 1

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

#define kZipIattrBinary 0
#define kZipIattrAscii  1

#define kZipCompressionNone    0
#define kZipCompressionDeflate 8

#define kZipCdirHdrMagic   0x06054b50 /* PK♣♠ "PK\5\6" */
#define kZipCdirHdrMinSize 22
#define kZipCdirAlign      64 /* our choice; actually 2 */
#define kZipCdirHdrLinkableSize \
  ROUNDUP(kZipCfileHdrMinSize + PATH_MAX, kZipCdirAlign)

#define kZipCfileHdrMagic                 0x02014b50 /* PK☺☻ "PK\1\2" */
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

#define kZipLfileHdrMagic                0x04034b50 /* PK♥♦ "PK\3\4" */
#define kZipLfileHdrMinSize              30
#define kZipLfileOffsetGeneralflag       6
#define kZipLfileOffsetCompressionmethod 8
#define kZipLfileOffsetLastmodifiedtime  10
#define kZipLfileOffsetLastmodifieddate  12
#define kZipLfileOffsetCrc32             14
#define kZipLfileOffsetCompressedsize    18

#define kZipGflagUtf8 0x800

#define kZipExtraHdrSize           4
#define kZipExtraZip64             0x0001
#define kZipExtraNtfs              0x000a
#define kZipExtraExtendedTimestamp 0x5455

#define kZipCfileMagic "PK\001\002"

#if !(__ASSEMBLER__ + __LINKER__ + 0)

/* end of central directory record */
#define ZIP_CDIR_MAGIC(P)         READ32LE(P)
#define ZIP_CDIR_DISK(P)          READ16LE((P) + 4)
#define ZIP_CDIR_STARTINGDISK(P)  READ16LE((P) + 6)
#define ZIP_CDIR_RECORDSONDISK(P) READ16LE((P) + 8)
#define ZIP_CDIR_RECORDS(P)       READ16LE((P) + 10)
#define ZIP_CDIR_SIZE(P)          READ32LE((P) + 12)
#define ZIP_CDIR_OFFSET(P)        READ32LE((P) + 16)
#define ZIP_CDIR_COMMENTSIZE(P)   READ16LE((P) + 20)
#define ZIP_CDIR_COMMENT(P)       (&(P)[22])
#define ZIP_CDIR_HDRSIZE(P)       (ZIP_CDIR_COMMENTSIZE(P) + kZipCdirHdrMinSize)

/* central directory file header */
#define ZIP_CFILE_MAGIC(P)          READ32LE(P)
#define ZIP_CFILE_VERSIONMADE(P)    ((P)[4])
#define ZIP_CFILE_FILEATTRCOMPAT(P) ((P)[5])
#define ZIP_CFILE_VERSIONNEED(P)    ((P)[6])
#define ZIP_CFILE_OSNEED(P)         ((P)[7])
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
#define ZIP_CFILE_NAME(P)   ((const char *)(&(P)[46])) /* not nul-terminated */
#define ZIP_CFILE_EXTRA(P)  (&(P)[46 + ZIP_CFILE_NAMESIZE(P)])
#define ZIP_CFILE_COMMENT(P) \
  (&(P)[46 + ZIP_CFILE_NAMESIZE(P) + ZIP_CFILE_EXTRASIZE(P)])
#define ZIP_CFILE_HDRSIZE(P)                                                   \
  (ZIP_CFILE_NAMESIZE(P) + ZIP_CFILE_EXTRASIZE(P) + ZIP_CFILE_COMMENTSIZE(P) + \
   kZipCfileHdrMinSize)

/* local file header */
#define ZIP_LFILE_MAGIC(P)       READ32LE(P)
#define ZIP_LFILE_VERSIONNEED(P) ((P)[4])
#define ZIP_LFILE_OSNEED(P)      ((P)[5])
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
#define ZIP_LFILE_UNCOMPRESSEDSIZE(P) READ32LE((P) + 22)
#define ZIP_LFILE_NAMESIZE(P)         READ16LE((P) + 26)
#define ZIP_LFILE_EXTRASIZE(P)        READ16LE((P) + 28)
#define ZIP_LFILE_NAME(P)             ((const char *)(&(P)[30]))
#define ZIP_LFILE_EXTRA(P)            (&(P)[30 + ZIP_LFILE_NAMESIZE(P)])
#define ZIP_LFILE_HDRSIZE(P) \
  (ZIP_LFILE_NAMESIZE(P) + ZIP_LFILE_EXTRASIZE(P) + kZipLfileHdrMinSize)
#define ZIP_LFILE_CONTENT(P) ((P) + ZIP_LFILE_HDRSIZE(P))
#define ZIP_LFILE_SIZE(P)    (ZIP_LFILE_HDRSIZE(P) + ZIP_LFILE_COMPRESSEDSIZE(P))

#define ZIP_EXTRA_HEADERID(P)    READ16LE(P)
#define ZIP_EXTRA_CONTENTSIZE(P) READ16LE((P) + 2)
#define ZIP_EXTRA_CONTENT(P)     (&(P)[4])
#define ZIP_EXTRA_SIZE(P)        (ZIP_EXTRA_CONTENTSIZE(P) + kZipExtraHdrSize)

uint8_t *zipfindcentraldir(const uint8_t *, size_t);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ZIP_H_ */
