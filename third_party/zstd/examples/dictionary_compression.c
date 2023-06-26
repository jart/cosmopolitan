// clang-format off
/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
**/

/* This example deals with Dictionary compression,
 * its counterpart is `examples/dictionary_decompression.c` .
 * These examples presume that a dictionary already exists.
 * The main method to create a dictionary is `zstd --train`,
 * look at the CLI documentation for details.
 * Another possible method is to employ dictionary training API,
 * published in `lib/zdict.h` .
**/

#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"     // printf
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/mem/alg.h"
#include "libc/mem/alloca.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/exit.h"
#include "third_party/getopt/getopt.h"
#include "third_party/musl/crypt.h"
#include "third_party/musl/rand48.h"    // free
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"    // memset, strcat
// MISSING #include <zstd.h>      // presumes zstd library is installed
#include "third_party/zstd/examples/common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()

/* createDict() :
** `dictFileName` is supposed already created using `zstd --train` */
static ZSTD_CDict* createCDict_orDie(const char* dictFileName, int cLevel)
{
    size_t dictSize;
    printf("loading dictionary %s \n", dictFileName);
    void* const dictBuffer = mallocAndLoadFile_orDie(dictFileName, &dictSize);
    ZSTD_CDict* const cdict = ZSTD_createCDict(dictBuffer, dictSize, cLevel);
    CHECK(cdict != NULL, "ZSTD_createCDict() failed!");
    free(dictBuffer);
    return cdict;
}


static void compress(const char* fname, const char* oname, const ZSTD_CDict* cdict)
{
    size_t fSize;
    void* const fBuff = mallocAndLoadFile_orDie(fname, &fSize);
    size_t const cBuffSize = ZSTD_compressBound(fSize);
    void* const cBuff = malloc_orDie(cBuffSize);

    /* Compress using the dictionary.
     * This function writes the dictionary id, and content size into the header.
     * But, it doesn't use a checksum. You can control these options using the
     * advanced API: ZSTD_CCtx_setParameter(), ZSTD_CCtx_refCDict(),
     * and ZSTD_compress2().
     */
    ZSTD_CCtx* const cctx = ZSTD_createCCtx();
    CHECK(cctx != NULL, "ZSTD_createCCtx() failed!");
    size_t const cSize = ZSTD_compress_usingCDict(cctx, cBuff, cBuffSize, fBuff, fSize, cdict);
    CHECK_ZSTD(cSize);

    saveFile_orDie(oname, cBuff, cSize);

    /* success */
    printf("%25s : %6u -> %7u - %s \n", fname, (unsigned)fSize, (unsigned)cSize, oname);

    ZSTD_freeCCtx(cctx);   /* never fails */
    free(fBuff);
    free(cBuff);
}


static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL + 5;
    void* outSpace = malloc_orDie(outL);
    memset(outSpace, 0, outL);
    strcat(outSpace, filename);
    strcat(outSpace, ".zst");
    return (char*)outSpace;
}

int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];
    int const cLevel = 3;

    if (argc<3) {
        fprintf(stderr, "wrong arguments\n");
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "%s [FILES] dictionary\n", exeName);
        return 1;
    }

    /* load dictionary only once */
    const char* const dictName = argv[argc-1];
    ZSTD_CDict* const dictPtr = createCDict_orDie(dictName, cLevel);

    int u;
    for (u=1; u<argc-1; u++) {
        const char* inFilename = argv[u];
        char* const outFilename = createOutFilename_orDie(inFilename);
        compress(inFilename, outFilename, dictPtr);
        free(outFilename);
    }

    ZSTD_freeCDict(dictPtr);
    printf("All %u files compressed. \n", argc-2);
    return 0;
}
