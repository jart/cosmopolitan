/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/zip.h"
#include "tool/decode/lib/zipnames.h"

const struct IdName kZipCompressionNames[] = {
    {kZipCompressionNone, "kZipCompressionNone"},
    {kZipCompressionDeflate, "kZipCompressionDeflate"},
    {0, 0},
};

const struct IdName kZipExtraNames[] = {
    {kZipExtraZip64, "kZipExtraZip64"},
    {kZipExtraNtfs, "kZipExtraNtfs"},
    {kZipExtraExtendedTimestamp, "kZipExtraExtendedTimestamp"},
    {0, 0},
};

const struct IdName kZipIattrNames[] = {
    {kZipIattrBinary, "kZipIattrBinary"},
    {kZipIattrAscii, "kZipIattrAscii"},
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
