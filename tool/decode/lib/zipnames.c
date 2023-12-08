/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/zip.internal.h"
#include "tool/decode/lib/zipnames.h"

const struct IdName kZipCompressionNames[] = {
    {kZipCompressionNone, "kZipCompressionNone"},
    {kZipCompressionDeflate, "kZipCompressionDeflate"},
    {0, 0},
};

const struct IdName kZipExtraNames[] = {
    {kZipExtraZip64, "kZipExtraZip64"},
    {kZipExtraNtfs, "kZipExtraNtfs"},
    {kZipExtraUnix, "kZipExtraUnix"},
    {kZipExtraExtendedTimestamp, "kZipExtraExtendedTimestamp"},
    {kZipExtraInfoZipNewUnixExtra, "kZipExtraInfoZipNewUnixExtra"},
    {0, 0},
};

const struct IdName kZipIattrNames[] = {
    {kZipIattrBinary, "kZipIattrBinary"},
    {kZipIattrText, "kZipIattrText"},
    {0, 0},
};

const struct IdName kZipOsNames[] = {
    {kZipOsDos, "kZipOsDos"},
    {kZipOsAmiga, "kZipOsAmiga"},
    {kZipOsOpenvms, "kZipOsOpenvms"},
    {kZipOsUnix, "kZipOsUnix"},
    {kZipOsVmcms, "kZipOsVmcms"},
    {kZipOsAtarist, "kZipOsAtarist"},
    {kZipOsOs2hpfs, "kZipOsOs2hpfs"},
    {kZipOsMacintosh, "kZipOsMacintosh"},
    {kZipOsZsystem, "kZipOsZsystem"},
    {kZipOsCpm, "kZipOsCpm"},
    {kZipOsWindowsntfs, "kZipOsWindowsntfs"},
    {kZipOsMvsos390zos, "kZipOsMvsos390zos"},
    {kZipOsVse, "kZipOsVse"},
    {kZipOsAcornrisc, "kZipOsAcornrisc"},
    {kZipOsVfat, "kZipOsVfat"},
    {kZipOsAltmvs, "kZipOsAltmvs"},
    {kZipOsBeos, "kZipOsBeos"},
    {kZipOsTandem, "kZipOsTandem"},
    {kZipOsOs400, "kZipOsOs400"},
    {kZipOsOsxdarwin, "kZipOsOsxdarwin"},
    {0, 0},
};

const struct IdName kZipEraNames[] = {
    {kZipEra1989, "kZipEra1989"},
    {kZipEra1993, "kZipEra1993"},
    {kZipEra2001, "kZipEra2001"},
    {0, 0},
};
