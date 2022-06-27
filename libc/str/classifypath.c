/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/str/path.h"
#include "libc/str/str.h"

/**
 * Classifies file path name.
 *
 * For the purposes of this function, we always consider backslash
 * interchangeable with forward slash, even though the underlying
 * operating system might not. Therefore, for the sake of clarity,
 * remaining documentation will only use the forward slash.
 *
 * This function behaves the same on all platforms. For instance, this
 * function will categorize `C:/FOO.BAR` as a DOS path, even if you're
 * running on UNIX rather than DOS.
 *
 * If you wish to check if a pathname is absolute, in a manner that's
 * inclusive of DOS drive paths, DOS rooted paths, in addition to the
 * New Technology UNC paths, then you may do the following:
 *
 *     if (_classifypath(str) & _kPathAbs) { ... }
 *
 * To check if path is a relative path:
 *
 *     if (~_classifypath(str) & _kPathAbs) { ... }
 *
 * Please note the above check includes rooted paths such as `\foo`
 * which is considered absolute by MSDN and we consider it absolute
 * although, it's technically relative to the current drive letter.
 *
 * Please note that `/foo/bar` is an absolute path on Windows, even
 * though it's actually a rooted path that's considered relative to
 * current drive by WIN32.
 *
 * @return integer value that's one of following:
 *     - `0` if non-weird relative path e.g. `c`
 *     - `_kPathAbs` if absolute (or rooted dos) path e.g. `/⋯`
 *     - `_kPathDos` if `c:`, `d:foo` i.e. drive-relative path
 *     - `_kPathAbs|_kPathDos` if proper dos path e.g. `c:/foo`
 *     - `_kPathDos|_kPathDev` if dos device path e.g. `nul`, `conin$`
 *     - `_kPathAbs|_kPathWin` if `//c`, `//?c`, etc.
 *     - `_kPathAbs|_kPathWin|_kPathDev` if `//./⋯`, `//?/⋯`
 *     - `_kPathAbs|_kPathWin|_kPathDev|_kPathRoot` if `//.` or `//?`
 *     - `_kPathAbs|_kPathNt` e.g. `\??\\⋯` (undoc. strict backslash)
 * @see "The Definitive Guide on Win32 to NT Path Conversion", James
 *     Forshaw, Google Project Zero Blog, 2016-02-29
 * @see "Naming Files, Paths, and Namespaces", MSDN 01/04/2021
 */
int _classifypath(const char *s) {
  if (s) {
    switch (s[0]) {
      case 0:  // ""
        return 0;
      default:
        if (!SupportsWindows()) {
          return 0;
        }
        if (((((s[0] == 'a' || s[0] == 'A') &&    // aux
               (s[1] == 'u' || s[1] == 'U') &&    //
               (s[2] == 'x' || s[2] == 'X')) ||   //
              ((s[0] == 'p' || s[0] == 'P') &&    // prn
               (s[1] == 'r' || s[1] == 'R') &&    //
               (s[2] == 'n' || s[2] == 'N')) ||   //
              ((s[0] == 'n' || s[0] == 'N') &&    // nul
               (s[1] == 'u' || s[1] == 'U') &&    //
               (s[2] == 'l' || s[2] == 'L')) ||   //
              ((s[0] == 'c' || s[0] == 'C') &&    // con
               (s[1] == 'o' || s[1] == 'O') &&    //
               (s[2] == 'n' || s[2] == 'N'))) &&  //
             !s[3]) ||
            ((((s[0] == 'l' || s[0] == 'L') &&    // lpt
               (s[1] == 'p' || s[1] == 'P') &&    //
               (s[2] == 't' || s[2] == 'T')) ||   //
              ((s[0] == 'c' || s[0] == 'C') &&    // com
               (s[1] == 'o' || s[1] == 'O') &&    //
               (s[2] == 'm' || s[2] == 'M'))) &&  //
             ('1' <= s[3] && s[3] <= '9') &&      //
             !s[4])) {
          return _kPathDos | _kPathDev;
        }
        switch (s[1]) {
          case ':':
            switch (s[2]) {
              case 0:   // c:
              default:  // c:wut⋯
                return _kPathDos;
              case '/':   // c:/⋯
              case '\\':  // c:\⋯
                return _kPathAbs | _kPathDos;
            }
          default:
            return 0;
        }
      case '\\':
        if (SupportsWindows()) {
          if (s[1] == '?' && s[2] == '?') {
            if (!s[3]) {
              return _kPathAbs | _kPathNt | _kPathRoot;  // \??\⋯
            } else if (s[3] == '\\') {
              return _kPathAbs | _kPathNt;  // \??\⋯
            }
          }
        }
        // fallthrough
      case '/':
        if (!SupportsWindows()) {
          return _kPathAbs;
        }
        switch (s[1]) {
          case 0:   // /
          default:  // /⋯
            return _kPathAbs;
          case '/':
          case '\\':
            switch (s[2]) {
              case 0:   // //
              default:  // //⋯
                return _kPathAbs | _kPathWin;
              case '.':
              case '?':
                switch (s[3]) {
                  case 0:  // //? or //.
                    return _kPathAbs | _kPathWin | _kPathDev | _kPathRoot;
                  default:  // //?⋯ or //.⋯
                    return _kPathAbs | _kPathWin;
                  case '/':
                  case '\\':  // //?/⋯ or //./⋯
                    return _kPathAbs | _kPathWin | _kPathDev;
                }
            }
        }
    }
  } else {
    return 0;
  }
}
