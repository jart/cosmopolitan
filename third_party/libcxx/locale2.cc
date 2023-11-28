//===------------------------- locale.cpp ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "third_party/libcxx/string"
#include "third_party/libcxx/locale"
#include "third_party/libcxx/codecvt"
#include "third_party/libcxx/vector"
#include "third_party/libcxx/algorithm"
#include "third_party/libcxx/typeinfo"
#ifndef _LIBCPP_NO_EXCEPTIONS
#include "third_party/libcxx/type_traits"
#endif
#include "third_party/libcxx/clocale"
#include "third_party/libcxx/cstring"
#include "third_party/libcxx/cwctype"
#include "third_party/libcxx/__sso_allocator"
#include "third_party/libcxx/include/atomic_support.hh"
#include "libc/str/locale.h"
#include "third_party/libcxx/countof.internal.hh"
#include "third_party/libcxx/__undef_macros"

// On Linux, wint_t and wchar_t have different signed-ness, and this causes
// lots of noise in the build log, but no bugs that I know of. 
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

namespace {

_LIBCPP_NORETURN static void __throw_runtime_error(const string &msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw runtime_error(msg);
#else
    (void)msg;
    _VSTD::abort();
#endif
}

} // namespace

struct __libcpp_unique_locale {
  __libcpp_unique_locale(const char* nm) : __loc_(newlocale(LC_ALL_MASK, nm, 0)) {}

  ~__libcpp_unique_locale() {
    if (__loc_)
      freelocale(__loc_);
  }

  explicit operator bool() const { return __loc_; }

  locale_t& get() { return __loc_; }

  locale_t __loc_;
private:
  __libcpp_unique_locale(__libcpp_unique_locale const&);
  __libcpp_unique_locale& operator=(__libcpp_unique_locale const&);
};

//                                     Valid UTF ranges
//     UTF-32               UTF-16                          UTF-8               # of code points
//                     first      second       first   second    third   fourth
// 000000 - 00007F  0000 - 007F               00 - 7F                                 127
// 000080 - 0007FF  0080 - 07FF               C2 - DF, 80 - BF                       1920
// 000800 - 000FFF  0800 - 0FFF               E0 - E0, A0 - BF, 80 - BF              2048
// 001000 - 00CFFF  1000 - CFFF               E1 - EC, 80 - BF, 80 - BF             49152
// 00D000 - 00D7FF  D000 - D7FF               ED - ED, 80 - 9F, 80 - BF              2048
// 00D800 - 00DFFF                invalid
// 00E000 - 00FFFF  E000 - FFFF               EE - EF, 80 - BF, 80 - BF              8192
// 010000 - 03FFFF  D800 - D8BF, DC00 - DFFF  F0 - F0, 90 - BF, 80 - BF, 80 - BF   196608
// 040000 - 0FFFFF  D8C0 - DBBF, DC00 - DFFF  F1 - F3, 80 - BF, 80 - BF, 80 - BF   786432
// 100000 - 10FFFF  DBC0 - DBFF, DC00 - DFFF  F4 - F4, 80 - 8F, 80 - BF, 80 - BF    65536

static
codecvt_base::result
utf16_to_utf8(const uint16_t* frm, const uint16_t* frm_end, const uint16_t*& frm_nxt,
              uint8_t* to, uint8_t* to_end, uint8_t*& to_nxt,
              unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & generate_header)
    {
        if (to_end-to_nxt < 3)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(0xEF);
        *to_nxt++ = static_cast<uint8_t>(0xBB);
        *to_nxt++ = static_cast<uint8_t>(0xBF);
    }
    for (; frm_nxt < frm_end; ++frm_nxt)
    {
        uint16_t wc1 = *frm_nxt;
        if (wc1 > Maxcode)
            return codecvt_base::error;
        if (wc1 < 0x0080)
        {
            if (to_end-to_nxt < 1)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(wc1);
        }
        else if (wc1 < 0x0800)
        {
            if (to_end-to_nxt < 2)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xC0 | (wc1 >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 | (wc1 & 0x03F));
        }
        else if (wc1 < 0xD800)
        {
            if (to_end-to_nxt < 3)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xE0 |  (wc1 >> 12));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc1 & 0x0FC0) >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 |  (wc1 & 0x003F));
        }
        else if (wc1 < 0xDC00)
        {
            if (frm_end-frm_nxt < 2)
                return codecvt_base::partial;
            uint16_t wc2 = frm_nxt[1];
            if ((wc2 & 0xFC00) != 0xDC00)
                return codecvt_base::error;
            if (to_end-to_nxt < 4)
                return codecvt_base::partial;
            if (((((wc1 & 0x03C0UL) >> 6) + 1) << 16) +
                ((wc1 & 0x003FUL) << 10) + (wc2 & 0x03FF) > Maxcode)
                return codecvt_base::error;
            ++frm_nxt;
            uint8_t z = ((wc1 & 0x03C0) >> 6) + 1;
            *to_nxt++ = static_cast<uint8_t>(0xF0 | (z >> 2));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((z & 0x03) << 4)     | ((wc1 & 0x003C) >> 2));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc1 & 0x0003) << 4) | ((wc2 & 0x03C0) >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 |  (wc2 & 0x003F));
        }
        else if (wc1 < 0xE000)
        {
            return codecvt_base::error;
        }
        else
        {
            if (to_end-to_nxt < 3)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xE0 |  (wc1 >> 12));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc1 & 0x0FC0) >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 |  (wc1 & 0x003F));
        }
    }
    return codecvt_base::ok;
}

static
codecvt_base::result
utf16_to_utf8(const uint32_t* frm, const uint32_t* frm_end, const uint32_t*& frm_nxt,
              uint8_t* to, uint8_t* to_end, uint8_t*& to_nxt,
              unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & generate_header)
    {
        if (to_end-to_nxt < 3)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(0xEF);
        *to_nxt++ = static_cast<uint8_t>(0xBB);
        *to_nxt++ = static_cast<uint8_t>(0xBF);
    }
    for (; frm_nxt < frm_end; ++frm_nxt)
    {
        uint16_t wc1 = static_cast<uint16_t>(*frm_nxt);
        if (wc1 > Maxcode)
            return codecvt_base::error;
        if (wc1 < 0x0080)
        {
            if (to_end-to_nxt < 1)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(wc1);
        }
        else if (wc1 < 0x0800)
        {
            if (to_end-to_nxt < 2)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xC0 | (wc1 >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 | (wc1 & 0x03F));
        }
        else if (wc1 < 0xD800)
        {
            if (to_end-to_nxt < 3)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xE0 |  (wc1 >> 12));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc1 & 0x0FC0) >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 |  (wc1 & 0x003F));
        }
        else if (wc1 < 0xDC00)
        {
            if (frm_end-frm_nxt < 2)
                return codecvt_base::partial;
            uint16_t wc2 = static_cast<uint16_t>(frm_nxt[1]);
            if ((wc2 & 0xFC00) != 0xDC00)
                return codecvt_base::error;
            if (to_end-to_nxt < 4)
                return codecvt_base::partial;
            if (((((wc1 & 0x03C0UL) >> 6) + 1) << 16) +
                ((wc1 & 0x003FUL) << 10) + (wc2 & 0x03FF) > Maxcode)
                return codecvt_base::error;
            ++frm_nxt;
            uint8_t z = ((wc1 & 0x03C0) >> 6) + 1;
            *to_nxt++ = static_cast<uint8_t>(0xF0 | (z >> 2));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((z & 0x03) << 4)     | ((wc1 & 0x003C) >> 2));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc1 & 0x0003) << 4) | ((wc2 & 0x03C0) >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 |  (wc2 & 0x003F));
        }
        else if (wc1 < 0xE000)
        {
            return codecvt_base::error;
        }
        else
        {
            if (to_end-to_nxt < 3)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xE0 |  (wc1 >> 12));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc1 & 0x0FC0) >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 |  (wc1 & 0x003F));
        }
    }
    return codecvt_base::ok;
}

static
codecvt_base::result
utf8_to_utf16(const uint8_t* frm, const uint8_t* frm_end, const uint8_t*& frm_nxt,
              uint16_t* to, uint16_t* to_end, uint16_t*& to_nxt,
              unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 3 && frm_nxt[0] == 0xEF && frm_nxt[1] == 0xBB &&
                                                          frm_nxt[2] == 0xBF)
            frm_nxt += 3;
    }
    for (; frm_nxt < frm_end && to_nxt < to_end; ++to_nxt)
    {
        uint8_t c1 = *frm_nxt;
        if (c1 > Maxcode)
            return codecvt_base::error;
        if (c1 < 0x80)
        {
            *to_nxt = static_cast<uint16_t>(c1);
            ++frm_nxt;
        }
        else if (c1 < 0xC2)
        {
            return codecvt_base::error;
        }
        else if (c1 < 0xE0)
        {
            if (frm_end-frm_nxt < 2)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            if ((c2 & 0xC0) != 0x80)
                return codecvt_base::error;
            uint16_t t = static_cast<uint16_t>(((c1 & 0x1F) << 6) | (c2 & 0x3F));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = t;
            frm_nxt += 2;
        }
        else if (c1 < 0xF0)
        {
            if (frm_end-frm_nxt < 3)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            switch (c1)
            {
            case 0xE0:
                if ((c2 & 0xE0) != 0xA0)
                    return codecvt_base::error;
                 break;
            case 0xED:
                if ((c2 & 0xE0) != 0x80)
                    return codecvt_base::error;
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return codecvt_base::error;
                 break;
            }
            if ((c3 & 0xC0) != 0x80)
                return codecvt_base::error;
            uint16_t t = static_cast<uint16_t>(((c1 & 0x0F) << 12)
                                             | ((c2 & 0x3F) << 6)
                                             |  (c3 & 0x3F));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = t;
            frm_nxt += 3;
        }
        else if (c1 < 0xF5)
        {
            if (frm_end-frm_nxt < 4)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            uint8_t c4 = frm_nxt[3];
            switch (c1)
            {
            case 0xF0:
                if (!(0x90 <= c2 && c2 <= 0xBF))
                    return codecvt_base::error;
                 break;
            case 0xF4:
                if ((c2 & 0xF0) != 0x80)
                    return codecvt_base::error;
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return codecvt_base::error;
                 break;
            }
            if ((c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80)
                return codecvt_base::error;
            if (to_end-to_nxt < 2)
                return codecvt_base::partial;
            if ((((c1 & 7UL) << 18) +
                ((c2 & 0x3FUL) << 12) +
                ((c3 & 0x3FUL) << 6) + (c4 & 0x3F)) > Maxcode)
                return codecvt_base::error;
            *to_nxt = static_cast<uint16_t>(
                    0xD800
                  | (((((c1 & 0x07) << 2) | ((c2 & 0x30) >> 4)) - 1) << 6)
                  | ((c2 & 0x0F) << 2)
                  | ((c3 & 0x30) >> 4));
            *++to_nxt = static_cast<uint16_t>(
                    0xDC00
                  | ((c3 & 0x0F) << 6)
                  |  (c4 & 0x3F));
            frm_nxt += 4;
        }
        else
        {
            return codecvt_base::error;
        }
    }
    return frm_nxt < frm_end ? codecvt_base::partial : codecvt_base::ok;
}

static
codecvt_base::result
utf8_to_utf16(const uint8_t* frm, const uint8_t* frm_end, const uint8_t*& frm_nxt,
              uint32_t* to, uint32_t* to_end, uint32_t*& to_nxt,
              unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 3 && frm_nxt[0] == 0xEF && frm_nxt[1] == 0xBB &&
                                                          frm_nxt[2] == 0xBF)
            frm_nxt += 3;
    }
    for (; frm_nxt < frm_end && to_nxt < to_end; ++to_nxt)
    {
        uint8_t c1 = *frm_nxt;
        if (c1 > Maxcode)
            return codecvt_base::error;
        if (c1 < 0x80)
        {
            *to_nxt = static_cast<uint32_t>(c1);
            ++frm_nxt;
        }
        else if (c1 < 0xC2)
        {
            return codecvt_base::error;
        }
        else if (c1 < 0xE0)
        {
            if (frm_end-frm_nxt < 2)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            if ((c2 & 0xC0) != 0x80)
                return codecvt_base::error;
            uint16_t t = static_cast<uint16_t>(((c1 & 0x1F) << 6) | (c2 & 0x3F));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = static_cast<uint32_t>(t);
            frm_nxt += 2;
        }
        else if (c1 < 0xF0)
        {
            if (frm_end-frm_nxt < 3)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            switch (c1)
            {
            case 0xE0:
                if ((c2 & 0xE0) != 0xA0)
                    return codecvt_base::error;
                 break;
            case 0xED:
                if ((c2 & 0xE0) != 0x80)
                    return codecvt_base::error;
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return codecvt_base::error;
                 break;
            }
            if ((c3 & 0xC0) != 0x80)
                return codecvt_base::error;
            uint16_t t = static_cast<uint16_t>(((c1 & 0x0F) << 12)
                                             | ((c2 & 0x3F) << 6)
                                             |  (c3 & 0x3F));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = static_cast<uint32_t>(t);
            frm_nxt += 3;
        }
        else if (c1 < 0xF5)
        {
            if (frm_end-frm_nxt < 4)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            uint8_t c4 = frm_nxt[3];
            switch (c1)
            {
            case 0xF0:
                if (!(0x90 <= c2 && c2 <= 0xBF))
                    return codecvt_base::error;
                 break;
            case 0xF4:
                if ((c2 & 0xF0) != 0x80)
                    return codecvt_base::error;
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return codecvt_base::error;
                 break;
            }
            if ((c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80)
                return codecvt_base::error;
            if (to_end-to_nxt < 2)
                return codecvt_base::partial;
            if ((((c1 & 7UL) << 18) +
                ((c2 & 0x3FUL) << 12) +
                ((c3 & 0x3FUL) << 6) + (c4 & 0x3F)) > Maxcode)
                return codecvt_base::error;
            *to_nxt = static_cast<uint32_t>(
                    0xD800
                  | (((((c1 & 0x07) << 2) | ((c2 & 0x30) >> 4)) - 1) << 6)
                  | ((c2 & 0x0F) << 2)
                  | ((c3 & 0x30) >> 4));
            *++to_nxt = static_cast<uint32_t>(
                    0xDC00
                  | ((c3 & 0x0F) << 6)
                  |  (c4 & 0x3F));
            frm_nxt += 4;
        }
        else
        {
            return codecvt_base::error;
        }
    }
    return frm_nxt < frm_end ? codecvt_base::partial : codecvt_base::ok;
}

static
int
utf8_to_utf16_length(const uint8_t* frm, const uint8_t* frm_end,
                     size_t mx, unsigned long Maxcode = 0x10FFFF,
                     codecvt_mode mode = codecvt_mode(0))
{
    const uint8_t* frm_nxt = frm;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 3 && frm_nxt[0] == 0xEF && frm_nxt[1] == 0xBB &&
                                                          frm_nxt[2] == 0xBF)
            frm_nxt += 3;
    }
    for (size_t nchar16_t = 0; frm_nxt < frm_end && nchar16_t < mx; ++nchar16_t)
    {
        uint8_t c1 = *frm_nxt;
        if (c1 > Maxcode)
            break;
        if (c1 < 0x80)
        {
            ++frm_nxt;
        }
        else if (c1 < 0xC2)
        {
            break;
        }
        else if (c1 < 0xE0)
        {
            if ((frm_end-frm_nxt < 2) || (frm_nxt[1] & 0xC0) != 0x80)
                break;
            uint16_t t = static_cast<uint16_t>(((c1 & 0x1F) << 6) | (frm_nxt[1] & 0x3F));
            if (t > Maxcode)
                break;
            frm_nxt += 2;
        }
        else if (c1 < 0xF0)
        {
            if (frm_end-frm_nxt < 3)
                break;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            switch (c1)
            {
            case 0xE0:
                if ((c2 & 0xE0) != 0xA0)
                    return static_cast<int>(frm_nxt - frm);
                break;
            case 0xED:
                if ((c2 & 0xE0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            }
            if ((c3 & 0xC0) != 0x80)
                break;
            if ((((c1 & 0x0Fu) << 12) | ((c2 & 0x3Fu) << 6) | (c3 & 0x3Fu)) > Maxcode)
                break;
            frm_nxt += 3;
        }
        else if (c1 < 0xF5)
        {
            if (frm_end-frm_nxt < 4 || mx-nchar16_t < 2)
                break;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            uint8_t c4 = frm_nxt[3];
            switch (c1)
            {
            case 0xF0:
                if (!(0x90 <= c2 && c2 <= 0xBF))
                    return static_cast<int>(frm_nxt - frm);
                 break;
            case 0xF4:
                if ((c2 & 0xF0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            }
            if ((c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80)
                break;
            if ((((c1 & 7UL) << 18) +
                ((c2 & 0x3FUL) << 12) +
                ((c3 & 0x3FUL) << 6) + (c4 & 0x3F)) > Maxcode)
                break;
            ++nchar16_t;
            frm_nxt += 4;
        }
        else
        {
            break;
        }
    }
    return static_cast<int>(frm_nxt - frm);
}

static
codecvt_base::result
ucs4_to_utf8(const uint32_t* frm, const uint32_t* frm_end, const uint32_t*& frm_nxt,
             uint8_t* to, uint8_t* to_end, uint8_t*& to_nxt,
             unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & generate_header)
    {
        if (to_end-to_nxt < 3)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(0xEF);
        *to_nxt++ = static_cast<uint8_t>(0xBB);
        *to_nxt++ = static_cast<uint8_t>(0xBF);
    }
    for (; frm_nxt < frm_end; ++frm_nxt)
    {
        uint32_t wc = *frm_nxt;
        if ((wc & 0xFFFFF800) == 0x00D800 || wc > Maxcode)
            return codecvt_base::error;
        if (wc < 0x000080)
        {
            if (to_end-to_nxt < 1)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(wc);
        }
        else if (wc < 0x000800)
        {
            if (to_end-to_nxt < 2)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xC0 | (wc >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 | (wc & 0x03F));
        }
        else if (wc < 0x010000)
        {
            if (to_end-to_nxt < 3)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xE0 |  (wc >> 12));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc & 0x0FC0) >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 |  (wc & 0x003F));
        }
        else // if (wc < 0x110000)
        {
            if (to_end-to_nxt < 4)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xF0 |  (wc >> 18));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc & 0x03F000) >> 12));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc & 0x000FC0) >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 |  (wc & 0x00003F));
        }
    }
    return codecvt_base::ok;
}

static
codecvt_base::result
utf8_to_ucs4(const uint8_t* frm, const uint8_t* frm_end, const uint8_t*& frm_nxt,
             uint32_t* to, uint32_t* to_end, uint32_t*& to_nxt,
             unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 3 && frm_nxt[0] == 0xEF && frm_nxt[1] == 0xBB &&
                                                          frm_nxt[2] == 0xBF)
            frm_nxt += 3;
    }
    for (; frm_nxt < frm_end && to_nxt < to_end; ++to_nxt)
    {
        uint8_t c1 = static_cast<uint8_t>(*frm_nxt);
        if (c1 < 0x80)
        {
            if (c1 > Maxcode)
                return codecvt_base::error;
            *to_nxt = static_cast<uint32_t>(c1);
            ++frm_nxt;
        }
        else if (c1 < 0xC2)
        {
            return codecvt_base::error;
        }
        else if (c1 < 0xE0)
        {
            if (frm_end-frm_nxt < 2)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            if ((c2 & 0xC0) != 0x80)
                return codecvt_base::error;
            uint32_t t = static_cast<uint32_t>(((c1 & 0x1F) << 6)
                                              | (c2 & 0x3F));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = t;
            frm_nxt += 2;
        }
        else if (c1 < 0xF0)
        {
            if (frm_end-frm_nxt < 3)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            switch (c1)
            {
            case 0xE0:
                if ((c2 & 0xE0) != 0xA0)
                    return codecvt_base::error;
                 break;
            case 0xED:
                if ((c2 & 0xE0) != 0x80)
                    return codecvt_base::error;
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return codecvt_base::error;
                 break;
            }
            if ((c3 & 0xC0) != 0x80)
                return codecvt_base::error;
            uint32_t t = static_cast<uint32_t>(((c1 & 0x0F) << 12)
                                             | ((c2 & 0x3F) << 6)
                                             |  (c3 & 0x3F));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = t;
            frm_nxt += 3;
        }
        else if (c1 < 0xF5)
        {
            if (frm_end-frm_nxt < 4)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            uint8_t c4 = frm_nxt[3];
            switch (c1)
            {
            case 0xF0:
                if (!(0x90 <= c2 && c2 <= 0xBF))
                    return codecvt_base::error;
                 break;
            case 0xF4:
                if ((c2 & 0xF0) != 0x80)
                    return codecvt_base::error;
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return codecvt_base::error;
                 break;
            }
            if ((c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80)
                return codecvt_base::error;
            uint32_t t = static_cast<uint32_t>(((c1 & 0x07) << 18)
                                             | ((c2 & 0x3F) << 12)
                                             | ((c3 & 0x3F) << 6)
                                             |  (c4 & 0x3F));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = t;
            frm_nxt += 4;
        }
        else
        {
            return codecvt_base::error;
        }
    }
    return frm_nxt < frm_end ? codecvt_base::partial : codecvt_base::ok;
}

static
int
utf8_to_ucs4_length(const uint8_t* frm, const uint8_t* frm_end,
                    size_t mx, unsigned long Maxcode = 0x10FFFF,
                    codecvt_mode mode = codecvt_mode(0))
{
    const uint8_t* frm_nxt = frm;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 3 && frm_nxt[0] == 0xEF && frm_nxt[1] == 0xBB &&
                                                          frm_nxt[2] == 0xBF)
            frm_nxt += 3;
    }
    for (size_t nchar32_t = 0; frm_nxt < frm_end && nchar32_t < mx; ++nchar32_t)
    {
        uint8_t c1 = static_cast<uint8_t>(*frm_nxt);
        if (c1 < 0x80)
        {
            if (c1 > Maxcode)
                break;
            ++frm_nxt;
        }
        else if (c1 < 0xC2)
        {
            break;
        }
        else if (c1 < 0xE0)
        {
            if ((frm_end-frm_nxt < 2) || ((frm_nxt[1] & 0xC0) != 0x80))
                break;
            if ((((c1 & 0x1Fu) << 6) | (frm_nxt[1] & 0x3Fu)) > Maxcode)
                break;
            frm_nxt += 2;
        }
        else if (c1 < 0xF0)
        {
            if (frm_end-frm_nxt < 3)
                break;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            switch (c1)
            {
            case 0xE0:
                if ((c2 & 0xE0) != 0xA0)
                    return static_cast<int>(frm_nxt - frm);
                break;
            case 0xED:
                if ((c2 & 0xE0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            }
            if ((c3 & 0xC0) != 0x80)
                break;
            if ((((c1 & 0x0Fu) << 12) | ((c2 & 0x3Fu) << 6) | (c3 & 0x3Fu)) > Maxcode)
                break;
            frm_nxt += 3;
        }
        else if (c1 < 0xF5)
        {
            if (frm_end-frm_nxt < 4)
                break;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            uint8_t c4 = frm_nxt[3];
            switch (c1)
            {
            case 0xF0:
                if (!(0x90 <= c2 && c2 <= 0xBF))
                    return static_cast<int>(frm_nxt - frm);
                 break;
            case 0xF4:
                if ((c2 & 0xF0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            }
            if ((c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80)
                break;
            if ((((c1 & 0x07u) << 18) | ((c2 & 0x3Fu) << 12) |
                 ((c3 & 0x3Fu) << 6)  |  (c4 & 0x3Fu)) > Maxcode)
                break;
            frm_nxt += 4;
        }
        else
        {
            break;
        }
    }
    return static_cast<int>(frm_nxt - frm);
}

static
codecvt_base::result
ucs2_to_utf8(const uint16_t* frm, const uint16_t* frm_end, const uint16_t*& frm_nxt,
             uint8_t* to, uint8_t* to_end, uint8_t*& to_nxt,
             unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & generate_header)
    {
        if (to_end-to_nxt < 3)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(0xEF);
        *to_nxt++ = static_cast<uint8_t>(0xBB);
        *to_nxt++ = static_cast<uint8_t>(0xBF);
    }
    for (; frm_nxt < frm_end; ++frm_nxt)
    {
        uint16_t wc = *frm_nxt;
        if ((wc & 0xF800) == 0xD800 || wc > Maxcode)
            return codecvt_base::error;
        if (wc < 0x0080)
        {
            if (to_end-to_nxt < 1)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(wc);
        }
        else if (wc < 0x0800)
        {
            if (to_end-to_nxt < 2)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xC0 | (wc >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 | (wc & 0x03F));
        }
        else // if (wc <= 0xFFFF)
        {
            if (to_end-to_nxt < 3)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(0xE0 |  (wc >> 12));
            *to_nxt++ = static_cast<uint8_t>(0x80 | ((wc & 0x0FC0) >> 6));
            *to_nxt++ = static_cast<uint8_t>(0x80 |  (wc & 0x003F));
        }
    }
    return codecvt_base::ok;
}

static
codecvt_base::result
utf8_to_ucs2(const uint8_t* frm, const uint8_t* frm_end, const uint8_t*& frm_nxt,
             uint16_t* to, uint16_t* to_end, uint16_t*& to_nxt,
             unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 3 && frm_nxt[0] == 0xEF && frm_nxt[1] == 0xBB &&
                                                          frm_nxt[2] == 0xBF)
            frm_nxt += 3;
    }
    for (; frm_nxt < frm_end && to_nxt < to_end; ++to_nxt)
    {
        uint8_t c1 = static_cast<uint8_t>(*frm_nxt);
        if (c1 < 0x80)
        {
            if (c1 > Maxcode)
                return codecvt_base::error;
            *to_nxt = static_cast<uint16_t>(c1);
            ++frm_nxt;
        }
        else if (c1 < 0xC2)
        {
            return codecvt_base::error;
        }
        else if (c1 < 0xE0)
        {
            if (frm_end-frm_nxt < 2)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            if ((c2 & 0xC0) != 0x80)
                return codecvt_base::error;
            uint16_t t = static_cast<uint16_t>(((c1 & 0x1F) << 6)
                                              | (c2 & 0x3F));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = t;
            frm_nxt += 2;
        }
        else if (c1 < 0xF0)
        {
            if (frm_end-frm_nxt < 3)
                return codecvt_base::partial;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            switch (c1)
            {
            case 0xE0:
                if ((c2 & 0xE0) != 0xA0)
                    return codecvt_base::error;
                 break;
            case 0xED:
                if ((c2 & 0xE0) != 0x80)
                    return codecvt_base::error;
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return codecvt_base::error;
                 break;
            }
            if ((c3 & 0xC0) != 0x80)
                return codecvt_base::error;
            uint16_t t = static_cast<uint16_t>(((c1 & 0x0F) << 12)
                                             | ((c2 & 0x3F) << 6)
                                             |  (c3 & 0x3F));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = t;
            frm_nxt += 3;
        }
        else
        {
            return codecvt_base::error;
        }
    }
    return frm_nxt < frm_end ? codecvt_base::partial : codecvt_base::ok;
}

static
int
utf8_to_ucs2_length(const uint8_t* frm, const uint8_t* frm_end,
                    size_t mx, unsigned long Maxcode = 0x10FFFF,
                    codecvt_mode mode = codecvt_mode(0))
{
    const uint8_t* frm_nxt = frm;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 3 && frm_nxt[0] == 0xEF && frm_nxt[1] == 0xBB &&
                                                          frm_nxt[2] == 0xBF)
            frm_nxt += 3;
    }
    for (size_t nchar32_t = 0; frm_nxt < frm_end && nchar32_t < mx; ++nchar32_t)
    {
        uint8_t c1 = static_cast<uint8_t>(*frm_nxt);
        if (c1 < 0x80)
        {
            if (c1 > Maxcode)
                break;
            ++frm_nxt;
        }
        else if (c1 < 0xC2)
        {
            break;
        }
        else if (c1 < 0xE0)
        {
            if ((frm_end-frm_nxt < 2) || ((frm_nxt[1] & 0xC0) != 0x80))
                break;
            if ((((c1 & 0x1Fu) << 6) | (frm_nxt[1] & 0x3Fu)) > Maxcode)
                break;
            frm_nxt += 2;
        }
        else if (c1 < 0xF0)
        {
            if (frm_end-frm_nxt < 3)
                break;
            uint8_t c2 = frm_nxt[1];
            uint8_t c3 = frm_nxt[2];
            switch (c1)
            {
            case 0xE0:
                if ((c2 & 0xE0) != 0xA0)
                    return static_cast<int>(frm_nxt - frm);
                break;
            case 0xED:
                if ((c2 & 0xE0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            default:
                if ((c2 & 0xC0) != 0x80)
                    return static_cast<int>(frm_nxt - frm);
                 break;
            }
            if ((c3 & 0xC0) != 0x80)
                break;
            if ((((c1 & 0x0Fu) << 12) | ((c2 & 0x3Fu) << 6) | (c3 & 0x3Fu)) > Maxcode)
                break;
            frm_nxt += 3;
        }
        else
        {
            break;
        }
    }
    return static_cast<int>(frm_nxt - frm);
}

static
codecvt_base::result
ucs4_to_utf16be(const uint32_t* frm, const uint32_t* frm_end, const uint32_t*& frm_nxt,
                uint8_t* to, uint8_t* to_end, uint8_t*& to_nxt,
                unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & generate_header)
    {
        if (to_end-to_nxt < 2)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(0xFE);
        *to_nxt++ = static_cast<uint8_t>(0xFF);
    }
    for (; frm_nxt < frm_end; ++frm_nxt)
    {
        uint32_t wc = *frm_nxt;
        if ((wc & 0xFFFFF800) == 0x00D800 || wc > Maxcode)
            return codecvt_base::error;
        if (wc < 0x010000)
        {
            if (to_end-to_nxt < 2)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(wc >> 8);
            *to_nxt++ = static_cast<uint8_t>(wc);
        }
        else
        {
            if (to_end-to_nxt < 4)
                return codecvt_base::partial;
            uint16_t t = static_cast<uint16_t>(
                    0xD800
                  | ((((wc & 0x1F0000) >> 16) - 1) << 6)
                  |   ((wc & 0x00FC00) >> 10));
            *to_nxt++ = static_cast<uint8_t>(t >> 8);
            *to_nxt++ = static_cast<uint8_t>(t);
            t = static_cast<uint16_t>(0xDC00 | (wc & 0x03FF));
            *to_nxt++ = static_cast<uint8_t>(t >> 8);
            *to_nxt++ = static_cast<uint8_t>(t);
        }
    }
    return codecvt_base::ok;
}

static
codecvt_base::result
utf16be_to_ucs4(const uint8_t* frm, const uint8_t* frm_end, const uint8_t*& frm_nxt,
                uint32_t* to, uint32_t* to_end, uint32_t*& to_nxt,
                unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 2 && frm_nxt[0] == 0xFE && frm_nxt[1] == 0xFF)
            frm_nxt += 2;
    }
    for (; frm_nxt < frm_end - 1 && to_nxt < to_end; ++to_nxt)
    {
        uint16_t c1 = static_cast<uint16_t>(frm_nxt[0] << 8 | frm_nxt[1]);
        if ((c1 & 0xFC00) == 0xDC00)
            return codecvt_base::error;
        if ((c1 & 0xFC00) != 0xD800)
        {
            if (c1 > Maxcode)
                return codecvt_base::error;
            *to_nxt = static_cast<uint32_t>(c1);
            frm_nxt += 2;
        }
        else
        {
            if (frm_end-frm_nxt < 4)
                return codecvt_base::partial;
            uint16_t c2 = static_cast<uint16_t>(frm_nxt[2] << 8 | frm_nxt[3]);
            if ((c2 & 0xFC00) != 0xDC00)
                return codecvt_base::error;
            uint32_t t = static_cast<uint32_t>(
                    ((((c1 & 0x03C0) >> 6) + 1) << 16)
                  |   ((c1 & 0x003F) << 10)
                  |    (c2 & 0x03FF));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = t;
            frm_nxt += 4;
        }
    }
    return frm_nxt < frm_end ? codecvt_base::partial : codecvt_base::ok;
}

static
int
utf16be_to_ucs4_length(const uint8_t* frm, const uint8_t* frm_end,
                       size_t mx, unsigned long Maxcode = 0x10FFFF,
                       codecvt_mode mode = codecvt_mode(0))
{
    const uint8_t* frm_nxt = frm;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 2 && frm_nxt[0] == 0xFE && frm_nxt[1] == 0xFF)
            frm_nxt += 2;
    }
    for (size_t nchar32_t = 0; frm_nxt < frm_end - 1 && nchar32_t < mx; ++nchar32_t)
    {
        uint16_t c1 = static_cast<uint16_t>(frm_nxt[0] << 8 | frm_nxt[1]);
        if ((c1 & 0xFC00) == 0xDC00)
            break;
        if ((c1 & 0xFC00) != 0xD800)
        {
            if (c1 > Maxcode)
                break;
            frm_nxt += 2;
        }
        else
        {
            if (frm_end-frm_nxt < 4)
                break;
            uint16_t c2 = static_cast<uint16_t>(frm_nxt[2] << 8 | frm_nxt[3]);
            if ((c2 & 0xFC00) != 0xDC00)
                break;
            uint32_t t = static_cast<uint32_t>(
                    ((((c1 & 0x03C0) >> 6) + 1) << 16)
                  |   ((c1 & 0x003F) << 10)
                  |    (c2 & 0x03FF));
            if (t > Maxcode)
                break;
            frm_nxt += 4;
        }
    }
    return static_cast<int>(frm_nxt - frm);
}

static
codecvt_base::result
ucs4_to_utf16le(const uint32_t* frm, const uint32_t* frm_end, const uint32_t*& frm_nxt,
                uint8_t* to, uint8_t* to_end, uint8_t*& to_nxt,
                unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & generate_header)
    {
        if (to_end - to_nxt < 2)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(0xFF);
        *to_nxt++ = static_cast<uint8_t>(0xFE);
    }
    for (; frm_nxt < frm_end; ++frm_nxt)
    {
        uint32_t wc = *frm_nxt;
        if ((wc & 0xFFFFF800) == 0x00D800 || wc > Maxcode)
            return codecvt_base::error;
        if (wc < 0x010000)
        {
            if (to_end-to_nxt < 2)
                return codecvt_base::partial;
            *to_nxt++ = static_cast<uint8_t>(wc);
            *to_nxt++ = static_cast<uint8_t>(wc >> 8);
        }
        else
        {
            if (to_end-to_nxt < 4)
                return codecvt_base::partial;
            uint16_t t = static_cast<uint16_t>(
                    0xD800
                  | ((((wc & 0x1F0000) >> 16) - 1) << 6)
                  |   ((wc & 0x00FC00) >> 10));
            *to_nxt++ = static_cast<uint8_t>(t);
            *to_nxt++ = static_cast<uint8_t>(t >> 8);
            t = static_cast<uint16_t>(0xDC00 | (wc & 0x03FF));
            *to_nxt++ = static_cast<uint8_t>(t);
            *to_nxt++ = static_cast<uint8_t>(t >> 8);
        }
    }
    return codecvt_base::ok;
}

static
codecvt_base::result
utf16le_to_ucs4(const uint8_t* frm, const uint8_t* frm_end, const uint8_t*& frm_nxt,
                uint32_t* to, uint32_t* to_end, uint32_t*& to_nxt,
                unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 2 && frm_nxt[0] == 0xFF && frm_nxt[1] == 0xFE)
            frm_nxt += 2;
    }
    for (; frm_nxt < frm_end - 1 && to_nxt < to_end; ++to_nxt)
    {
        uint16_t c1 = static_cast<uint16_t>(frm_nxt[1] << 8 | frm_nxt[0]);
        if ((c1 & 0xFC00) == 0xDC00)
            return codecvt_base::error;
        if ((c1 & 0xFC00) != 0xD800)
        {
            if (c1 > Maxcode)
                return codecvt_base::error;
            *to_nxt = static_cast<uint32_t>(c1);
            frm_nxt += 2;
        }
        else
        {
            if (frm_end-frm_nxt < 4)
                return codecvt_base::partial;
            uint16_t c2 = static_cast<uint16_t>(frm_nxt[3] << 8 | frm_nxt[2]);
            if ((c2 & 0xFC00) != 0xDC00)
                return codecvt_base::error;
            uint32_t t = static_cast<uint32_t>(
                    ((((c1 & 0x03C0) >> 6) + 1) << 16)
                  |   ((c1 & 0x003F) << 10)
                  |    (c2 & 0x03FF));
            if (t > Maxcode)
                return codecvt_base::error;
            *to_nxt = t;
            frm_nxt += 4;
        }
    }
    return frm_nxt < frm_end ? codecvt_base::partial : codecvt_base::ok;
}

static
int
utf16le_to_ucs4_length(const uint8_t* frm, const uint8_t* frm_end,
                       size_t mx, unsigned long Maxcode = 0x10FFFF,
                       codecvt_mode mode = codecvt_mode(0))
{
    const uint8_t* frm_nxt = frm;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 2 && frm_nxt[0] == 0xFF && frm_nxt[1] == 0xFE)
            frm_nxt += 2;
    }
    for (size_t nchar32_t = 0; frm_nxt < frm_end - 1 && nchar32_t < mx; ++nchar32_t)
    {
        uint16_t c1 = static_cast<uint16_t>(frm_nxt[1] << 8 | frm_nxt[0]);
        if ((c1 & 0xFC00) == 0xDC00)
            break;
        if ((c1 & 0xFC00) != 0xD800)
        {
            if (c1 > Maxcode)
                break;
            frm_nxt += 2;
        }
        else
        {
            if (frm_end-frm_nxt < 4)
                break;
            uint16_t c2 = static_cast<uint16_t>(frm_nxt[3] << 8 | frm_nxt[2]);
            if ((c2 & 0xFC00) != 0xDC00)
                break;
            uint32_t t = static_cast<uint32_t>(
                    ((((c1 & 0x03C0) >> 6) + 1) << 16)
                  |   ((c1 & 0x003F) << 10)
                  |    (c2 & 0x03FF));
            if (t > Maxcode)
                break;
            frm_nxt += 4;
        }
    }
    return static_cast<int>(frm_nxt - frm);
}

static
codecvt_base::result
ucs2_to_utf16be(const uint16_t* frm, const uint16_t* frm_end, const uint16_t*& frm_nxt,
                uint8_t* to, uint8_t* to_end, uint8_t*& to_nxt,
                unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & generate_header)
    {
        if (to_end-to_nxt < 2)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(0xFE);
        *to_nxt++ = static_cast<uint8_t>(0xFF);
    }
    for (; frm_nxt < frm_end; ++frm_nxt)
    {
        uint16_t wc = *frm_nxt;
        if ((wc & 0xF800) == 0xD800 || wc > Maxcode)
            return codecvt_base::error;
        if (to_end-to_nxt < 2)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(wc >> 8);
        *to_nxt++ = static_cast<uint8_t>(wc);
    }
    return codecvt_base::ok;
}

static
codecvt_base::result
utf16be_to_ucs2(const uint8_t* frm, const uint8_t* frm_end, const uint8_t*& frm_nxt,
                uint16_t* to, uint16_t* to_end, uint16_t*& to_nxt,
                unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 2 && frm_nxt[0] == 0xFE && frm_nxt[1] == 0xFF)
            frm_nxt += 2;
    }
    for (; frm_nxt < frm_end - 1 && to_nxt < to_end; ++to_nxt)
    {
        uint16_t c1 = static_cast<uint16_t>(frm_nxt[0] << 8 | frm_nxt[1]);
        if ((c1 & 0xF800) == 0xD800 || c1 > Maxcode)
            return codecvt_base::error;
        *to_nxt = c1;
        frm_nxt += 2;
    }
    return frm_nxt < frm_end ? codecvt_base::partial : codecvt_base::ok;
}

static
int
utf16be_to_ucs2_length(const uint8_t* frm, const uint8_t* frm_end,
                       size_t mx, unsigned long Maxcode = 0x10FFFF,
                       codecvt_mode mode = codecvt_mode(0))
{
    const uint8_t* frm_nxt = frm;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 2 && frm_nxt[0] == 0xFE && frm_nxt[1] == 0xFF)
            frm_nxt += 2;
    }
    for (size_t nchar16_t = 0; frm_nxt < frm_end - 1 && nchar16_t < mx; ++nchar16_t)
    {
        uint16_t c1 = static_cast<uint16_t>(frm_nxt[0] << 8 | frm_nxt[1]);
        if ((c1 & 0xF800) == 0xD800 || c1 > Maxcode)
            break;
        frm_nxt += 2;
    }
    return static_cast<int>(frm_nxt - frm);
}

static
codecvt_base::result
ucs2_to_utf16le(const uint16_t* frm, const uint16_t* frm_end, const uint16_t*& frm_nxt,
                uint8_t* to, uint8_t* to_end, uint8_t*& to_nxt,
                unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & generate_header)
    {
        if (to_end-to_nxt < 2)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(0xFF);
        *to_nxt++ = static_cast<uint8_t>(0xFE);
    }
    for (; frm_nxt < frm_end; ++frm_nxt)
    {
        uint16_t wc = *frm_nxt;
        if ((wc & 0xF800) == 0xD800 || wc > Maxcode)
            return codecvt_base::error;
        if (to_end-to_nxt < 2)
            return codecvt_base::partial;
        *to_nxt++ = static_cast<uint8_t>(wc);
        *to_nxt++ = static_cast<uint8_t>(wc >> 8);
    }
    return codecvt_base::ok;
}

static
codecvt_base::result
utf16le_to_ucs2(const uint8_t* frm, const uint8_t* frm_end, const uint8_t*& frm_nxt,
                uint16_t* to, uint16_t* to_end, uint16_t*& to_nxt,
                unsigned long Maxcode = 0x10FFFF, codecvt_mode mode = codecvt_mode(0))
{
    frm_nxt = frm;
    to_nxt = to;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 2 && frm_nxt[0] == 0xFF && frm_nxt[1] == 0xFE)
            frm_nxt += 2;
    }
    for (; frm_nxt < frm_end - 1 && to_nxt < to_end; ++to_nxt)
    {
        uint16_t c1 = static_cast<uint16_t>(frm_nxt[1] << 8 | frm_nxt[0]);
        if ((c1 & 0xF800) == 0xD800 || c1 > Maxcode)
            return codecvt_base::error;
        *to_nxt = c1;
        frm_nxt += 2;
    }
    return frm_nxt < frm_end ? codecvt_base::partial : codecvt_base::ok;
}

static
int
utf16le_to_ucs2_length(const uint8_t* frm, const uint8_t* frm_end,
                       size_t mx, unsigned long Maxcode = 0x10FFFF,
                       codecvt_mode mode = codecvt_mode(0))
{
    const uint8_t* frm_nxt = frm;
    frm_nxt = frm;
    if (mode & consume_header)
    {
        if (frm_end-frm_nxt >= 2 && frm_nxt[0] == 0xFF && frm_nxt[1] == 0xFE)
            frm_nxt += 2;
    }
    for (size_t nchar16_t = 0; frm_nxt < frm_end - 1 && nchar16_t < mx; ++nchar16_t)
    {
        uint16_t c1 = static_cast<uint16_t>(frm_nxt[1] << 8 | frm_nxt[0]);
        if ((c1 & 0xF800) == 0xD800 || c1 > Maxcode)
            break;
        frm_nxt += 2;
    }
    return static_cast<int>(frm_nxt - frm);
}

// template <> class codecvt<char16_t, char, mbstate_t>

locale::id codecvt<char16_t, char, mbstate_t>::id;

codecvt<char16_t, char, mbstate_t>::~codecvt()
{
}

codecvt<char16_t, char, mbstate_t>::result
codecvt<char16_t, char, mbstate_t>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint16_t* _frm = reinterpret_cast<const uint16_t*>(frm);
    const uint16_t* _frm_end = reinterpret_cast<const uint16_t*>(frm_end);
    const uint16_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = utf16_to_utf8(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

codecvt<char16_t, char, mbstate_t>::result
codecvt<char16_t, char, mbstate_t>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint16_t* _to = reinterpret_cast<uint16_t*>(to);
    uint16_t* _to_end = reinterpret_cast<uint16_t*>(to_end);
    uint16_t* _to_nxt = _to;
    result r = utf8_to_utf16(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

codecvt<char16_t, char, mbstate_t>::result
codecvt<char16_t, char, mbstate_t>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
codecvt<char16_t, char, mbstate_t>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
codecvt<char16_t, char, mbstate_t>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
codecvt<char16_t, char, mbstate_t>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf8_to_utf16_length(_frm, _frm_end, mx);
}

int
codecvt<char16_t, char, mbstate_t>::do_max_length() const  _NOEXCEPT
{
    return 4;
}

// template <> class codecvt<char32_t, char, mbstate_t>

locale::id codecvt<char32_t, char, mbstate_t>::id;

codecvt<char32_t, char, mbstate_t>::~codecvt()
{
}

codecvt<char32_t, char, mbstate_t>::result
codecvt<char32_t, char, mbstate_t>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint32_t* _frm = reinterpret_cast<const uint32_t*>(frm);
    const uint32_t* _frm_end = reinterpret_cast<const uint32_t*>(frm_end);
    const uint32_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = ucs4_to_utf8(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

codecvt<char32_t, char, mbstate_t>::result
codecvt<char32_t, char, mbstate_t>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint32_t* _to = reinterpret_cast<uint32_t*>(to);
    uint32_t* _to_end = reinterpret_cast<uint32_t*>(to_end);
    uint32_t* _to_nxt = _to;
    result r = utf8_to_ucs4(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

codecvt<char32_t, char, mbstate_t>::result
codecvt<char32_t, char, mbstate_t>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
codecvt<char32_t, char, mbstate_t>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
codecvt<char32_t, char, mbstate_t>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
codecvt<char32_t, char, mbstate_t>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf8_to_ucs4_length(_frm, _frm_end, mx);
}

int
codecvt<char32_t, char, mbstate_t>::do_max_length() const  _NOEXCEPT
{
    return 4;
}

// __codecvt_utf8<wchar_t>

__codecvt_utf8<wchar_t>::result
__codecvt_utf8<wchar_t>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
#if defined(_LIBCPP_SHORT_WCHAR)
    const uint16_t* _frm = reinterpret_cast<const uint16_t*>(frm);
    const uint16_t* _frm_end = reinterpret_cast<const uint16_t*>(frm_end);
    const uint16_t* _frm_nxt = _frm;
#else
    const uint32_t* _frm = reinterpret_cast<const uint32_t*>(frm);
    const uint32_t* _frm_end = reinterpret_cast<const uint32_t*>(frm_end);
    const uint32_t* _frm_nxt = _frm;
#endif
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
#if defined(_LIBCPP_SHORT_WCHAR)
    result r = ucs2_to_utf8(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                            _Maxcode_, _Mode_);
#else
    result r = ucs4_to_utf8(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                            _Maxcode_, _Mode_);
#endif
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8<wchar_t>::result
__codecvt_utf8<wchar_t>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
#if defined(_LIBCPP_SHORT_WCHAR)
    uint16_t* _to = reinterpret_cast<uint16_t*>(to);
    uint16_t* _to_end = reinterpret_cast<uint16_t*>(to_end);
    uint16_t* _to_nxt = _to;
    result r = utf8_to_ucs2(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                            _Maxcode_, _Mode_);
#else
    uint32_t* _to = reinterpret_cast<uint32_t*>(to);
    uint32_t* _to_end = reinterpret_cast<uint32_t*>(to_end);
    uint32_t* _to_nxt = _to;
    result r = utf8_to_ucs4(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                            _Maxcode_, _Mode_);
#endif
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8<wchar_t>::result
__codecvt_utf8<wchar_t>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf8<wchar_t>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf8<wchar_t>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf8<wchar_t>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf8_to_ucs4_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf8<wchar_t>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 7;
    return 4;
}

// __codecvt_utf8<char16_t>

__codecvt_utf8<char16_t>::result
__codecvt_utf8<char16_t>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint16_t* _frm = reinterpret_cast<const uint16_t*>(frm);
    const uint16_t* _frm_end = reinterpret_cast<const uint16_t*>(frm_end);
    const uint16_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = ucs2_to_utf8(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                            _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8<char16_t>::result
__codecvt_utf8<char16_t>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint16_t* _to = reinterpret_cast<uint16_t*>(to);
    uint16_t* _to_end = reinterpret_cast<uint16_t*>(to_end);
    uint16_t* _to_nxt = _to;
    result r = utf8_to_ucs2(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                            _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8<char16_t>::result
__codecvt_utf8<char16_t>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf8<char16_t>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf8<char16_t>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf8<char16_t>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf8_to_ucs2_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf8<char16_t>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 6;
    return 3;
}

// __codecvt_utf8<char32_t>

__codecvt_utf8<char32_t>::result
__codecvt_utf8<char32_t>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint32_t* _frm = reinterpret_cast<const uint32_t*>(frm);
    const uint32_t* _frm_end = reinterpret_cast<const uint32_t*>(frm_end);
    const uint32_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = ucs4_to_utf8(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                            _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8<char32_t>::result
__codecvt_utf8<char32_t>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint32_t* _to = reinterpret_cast<uint32_t*>(to);
    uint32_t* _to_end = reinterpret_cast<uint32_t*>(to_end);
    uint32_t* _to_nxt = _to;
    result r = utf8_to_ucs4(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                            _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8<char32_t>::result
__codecvt_utf8<char32_t>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf8<char32_t>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf8<char32_t>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf8<char32_t>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf8_to_ucs4_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf8<char32_t>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 7;
    return 4;
}

// __codecvt_utf16<wchar_t, false>

__codecvt_utf16<wchar_t, false>::result
__codecvt_utf16<wchar_t, false>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint32_t* _frm = reinterpret_cast<const uint32_t*>(frm);
    const uint32_t* _frm_end = reinterpret_cast<const uint32_t*>(frm_end);
    const uint32_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = ucs4_to_utf16be(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<wchar_t, false>::result
__codecvt_utf16<wchar_t, false>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint32_t* _to = reinterpret_cast<uint32_t*>(to);
    uint32_t* _to_end = reinterpret_cast<uint32_t*>(to_end);
    uint32_t* _to_nxt = _to;
    result r = utf16be_to_ucs4(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<wchar_t, false>::result
__codecvt_utf16<wchar_t, false>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf16<wchar_t, false>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf16<wchar_t, false>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf16<wchar_t, false>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf16be_to_ucs4_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf16<wchar_t, false>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 6;
    return 4;
}

// __codecvt_utf16<wchar_t, true>

__codecvt_utf16<wchar_t, true>::result
__codecvt_utf16<wchar_t, true>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint32_t* _frm = reinterpret_cast<const uint32_t*>(frm);
    const uint32_t* _frm_end = reinterpret_cast<const uint32_t*>(frm_end);
    const uint32_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = ucs4_to_utf16le(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<wchar_t, true>::result
__codecvt_utf16<wchar_t, true>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint32_t* _to = reinterpret_cast<uint32_t*>(to);
    uint32_t* _to_end = reinterpret_cast<uint32_t*>(to_end);
    uint32_t* _to_nxt = _to;
    result r = utf16le_to_ucs4(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<wchar_t, true>::result
__codecvt_utf16<wchar_t, true>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf16<wchar_t, true>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf16<wchar_t, true>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf16<wchar_t, true>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf16le_to_ucs4_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf16<wchar_t, true>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 6;
    return 4;
}

// __codecvt_utf16<char16_t, false>

__codecvt_utf16<char16_t, false>::result
__codecvt_utf16<char16_t, false>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint16_t* _frm = reinterpret_cast<const uint16_t*>(frm);
    const uint16_t* _frm_end = reinterpret_cast<const uint16_t*>(frm_end);
    const uint16_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = ucs2_to_utf16be(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<char16_t, false>::result
__codecvt_utf16<char16_t, false>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint16_t* _to = reinterpret_cast<uint16_t*>(to);
    uint16_t* _to_end = reinterpret_cast<uint16_t*>(to_end);
    uint16_t* _to_nxt = _to;
    result r = utf16be_to_ucs2(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<char16_t, false>::result
__codecvt_utf16<char16_t, false>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf16<char16_t, false>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf16<char16_t, false>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf16<char16_t, false>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf16be_to_ucs2_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf16<char16_t, false>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 4;
    return 2;
}

// __codecvt_utf16<char16_t, true>

__codecvt_utf16<char16_t, true>::result
__codecvt_utf16<char16_t, true>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint16_t* _frm = reinterpret_cast<const uint16_t*>(frm);
    const uint16_t* _frm_end = reinterpret_cast<const uint16_t*>(frm_end);
    const uint16_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = ucs2_to_utf16le(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<char16_t, true>::result
__codecvt_utf16<char16_t, true>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint16_t* _to = reinterpret_cast<uint16_t*>(to);
    uint16_t* _to_end = reinterpret_cast<uint16_t*>(to_end);
    uint16_t* _to_nxt = _to;
    result r = utf16le_to_ucs2(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<char16_t, true>::result
__codecvt_utf16<char16_t, true>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf16<char16_t, true>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf16<char16_t, true>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf16<char16_t, true>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf16le_to_ucs2_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf16<char16_t, true>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 4;
    return 2;
}

// __codecvt_utf16<char32_t, false>

__codecvt_utf16<char32_t, false>::result
__codecvt_utf16<char32_t, false>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint32_t* _frm = reinterpret_cast<const uint32_t*>(frm);
    const uint32_t* _frm_end = reinterpret_cast<const uint32_t*>(frm_end);
    const uint32_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = ucs4_to_utf16be(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<char32_t, false>::result
__codecvt_utf16<char32_t, false>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint32_t* _to = reinterpret_cast<uint32_t*>(to);
    uint32_t* _to_end = reinterpret_cast<uint32_t*>(to_end);
    uint32_t* _to_nxt = _to;
    result r = utf16be_to_ucs4(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<char32_t, false>::result
__codecvt_utf16<char32_t, false>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf16<char32_t, false>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf16<char32_t, false>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf16<char32_t, false>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf16be_to_ucs4_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf16<char32_t, false>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 6;
    return 4;
}

// __codecvt_utf16<char32_t, true>

__codecvt_utf16<char32_t, true>::result
__codecvt_utf16<char32_t, true>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint32_t* _frm = reinterpret_cast<const uint32_t*>(frm);
    const uint32_t* _frm_end = reinterpret_cast<const uint32_t*>(frm_end);
    const uint32_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = ucs4_to_utf16le(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<char32_t, true>::result
__codecvt_utf16<char32_t, true>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint32_t* _to = reinterpret_cast<uint32_t*>(to);
    uint32_t* _to_end = reinterpret_cast<uint32_t*>(to_end);
    uint32_t* _to_nxt = _to;
    result r = utf16le_to_ucs4(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                               _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf16<char32_t, true>::result
__codecvt_utf16<char32_t, true>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf16<char32_t, true>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf16<char32_t, true>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf16<char32_t, true>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf16le_to_ucs4_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf16<char32_t, true>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 6;
    return 4;
}

// __codecvt_utf8_utf16<wchar_t>

__codecvt_utf8_utf16<wchar_t>::result
__codecvt_utf8_utf16<wchar_t>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint32_t* _frm = reinterpret_cast<const uint32_t*>(frm);
    const uint32_t* _frm_end = reinterpret_cast<const uint32_t*>(frm_end);
    const uint32_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = utf16_to_utf8(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                             _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8_utf16<wchar_t>::result
__codecvt_utf8_utf16<wchar_t>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint32_t* _to = reinterpret_cast<uint32_t*>(to);
    uint32_t* _to_end = reinterpret_cast<uint32_t*>(to_end);
    uint32_t* _to_nxt = _to;
    result r = utf8_to_utf16(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                             _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8_utf16<wchar_t>::result
__codecvt_utf8_utf16<wchar_t>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf8_utf16<wchar_t>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf8_utf16<wchar_t>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf8_utf16<wchar_t>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf8_to_utf16_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf8_utf16<wchar_t>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 7;
    return 4;
}

// __codecvt_utf8_utf16<char16_t>

__codecvt_utf8_utf16<char16_t>::result
__codecvt_utf8_utf16<char16_t>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint16_t* _frm = reinterpret_cast<const uint16_t*>(frm);
    const uint16_t* _frm_end = reinterpret_cast<const uint16_t*>(frm_end);
    const uint16_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = utf16_to_utf8(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                             _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8_utf16<char16_t>::result
__codecvt_utf8_utf16<char16_t>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint16_t* _to = reinterpret_cast<uint16_t*>(to);
    uint16_t* _to_end = reinterpret_cast<uint16_t*>(to_end);
    uint16_t* _to_nxt = _to;
    result r = utf8_to_utf16(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                             _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8_utf16<char16_t>::result
__codecvt_utf8_utf16<char16_t>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf8_utf16<char16_t>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf8_utf16<char16_t>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf8_utf16<char16_t>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf8_to_utf16_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf8_utf16<char16_t>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 7;
    return 4;
}

// __codecvt_utf8_utf16<char32_t>

__codecvt_utf8_utf16<char32_t>::result
__codecvt_utf8_utf16<char32_t>::do_out(state_type&,
    const intern_type* frm, const intern_type* frm_end, const intern_type*& frm_nxt,
    extern_type* to, extern_type* to_end, extern_type*& to_nxt) const
{
    const uint32_t* _frm = reinterpret_cast<const uint32_t*>(frm);
    const uint32_t* _frm_end = reinterpret_cast<const uint32_t*>(frm_end);
    const uint32_t* _frm_nxt = _frm;
    uint8_t* _to = reinterpret_cast<uint8_t*>(to);
    uint8_t* _to_end = reinterpret_cast<uint8_t*>(to_end);
    uint8_t* _to_nxt = _to;
    result r = utf16_to_utf8(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                             _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8_utf16<char32_t>::result
__codecvt_utf8_utf16<char32_t>::do_in(state_type&,
    const extern_type* frm, const extern_type* frm_end, const extern_type*& frm_nxt,
    intern_type* to, intern_type* to_end, intern_type*& to_nxt) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    const uint8_t* _frm_nxt = _frm;
    uint32_t* _to = reinterpret_cast<uint32_t*>(to);
    uint32_t* _to_end = reinterpret_cast<uint32_t*>(to_end);
    uint32_t* _to_nxt = _to;
    result r = utf8_to_utf16(_frm, _frm_end, _frm_nxt, _to, _to_end, _to_nxt,
                             _Maxcode_, _Mode_);
    frm_nxt = frm + (_frm_nxt - _frm);
    to_nxt = to + (_to_nxt - _to);
    return r;
}

__codecvt_utf8_utf16<char32_t>::result
__codecvt_utf8_utf16<char32_t>::do_unshift(state_type&,
    extern_type* to, extern_type*, extern_type*& to_nxt) const
{
    to_nxt = to;
    return noconv;
}

int
__codecvt_utf8_utf16<char32_t>::do_encoding() const  _NOEXCEPT
{
    return 0;
}

bool
__codecvt_utf8_utf16<char32_t>::do_always_noconv() const  _NOEXCEPT
{
    return false;
}

int
__codecvt_utf8_utf16<char32_t>::do_length(state_type&,
    const extern_type* frm, const extern_type* frm_end, size_t mx) const
{
    const uint8_t* _frm = reinterpret_cast<const uint8_t*>(frm);
    const uint8_t* _frm_end = reinterpret_cast<const uint8_t*>(frm_end);
    return utf8_to_utf16_length(_frm, _frm_end, mx, _Maxcode_, _Mode_);
}

int
__codecvt_utf8_utf16<char32_t>::do_max_length() const  _NOEXCEPT
{
    if (_Mode_ & consume_header)
        return 7;
    return 4;
}

// __narrow_to_utf8<16>

__narrow_to_utf8<16>::~__narrow_to_utf8()
{
}

// __narrow_to_utf8<32>

__narrow_to_utf8<32>::~__narrow_to_utf8()
{
}

// __widen_from_utf8<16>

__widen_from_utf8<16>::~__widen_from_utf8()
{
}

// __widen_from_utf8<32>

__widen_from_utf8<32>::~__widen_from_utf8()
{
}


bool __checked_string_to_wchar_convert(wchar_t& dest,
                                       const char* ptr,
                                       locale_t loc) {
  if (*ptr == '\0')
    return false;
  mbstate_t mb = {};
  wchar_t out;
  size_t ret = __libcpp_mbrtowc_l(&out, ptr, strlen(ptr), &mb, loc);
  if (ret == static_cast<size_t>(-1) || ret == static_cast<size_t>(-2)) {
    return false;
  }
  dest = out;
  return true;
}

bool __checked_string_to_char_convert(char& dest,
                                      const char* ptr,
                                      locale_t __loc) {
  if (*ptr == '\0')
    return false;
  if (!ptr[1]) {
    dest = *ptr;
    return true;
  }
  // First convert the MBS into a wide char then attempt to narrow it using
  // wctob_l.
  wchar_t wout;
  if (!__checked_string_to_wchar_convert(wout, ptr, __loc))
    return false;
  int res;
  if ((res = __libcpp_wctob_l(wout, __loc)) != char_traits<char>::eof()) {
    dest = res;
    return true;
  }
  // FIXME: Work around specific multibyte sequences that we can reasonable
  // translate into a different single byte.
  switch (wout) {
  case L'\u202F': // narrow non-breaking space
  case L'\u00A0': // non-breaking space
    dest = ' ';
    return true;
  default:
    return false;
  }
  _LIBCPP_UNREACHABLE();
}


// numpunct<char> && numpunct<wchar_t>

locale::id numpunct< char  >::id;
locale::id numpunct<wchar_t>::id;

numpunct<char>::numpunct(size_t refs)
    : locale::facet(refs),
      __decimal_point_('.'),
      __thousands_sep_(',')
{
}

numpunct<wchar_t>::numpunct(size_t refs)
    : locale::facet(refs),
      __decimal_point_(L'.'),
      __thousands_sep_(L',')
{
}

numpunct<char>::~numpunct()
{
}

numpunct<wchar_t>::~numpunct()
{
}

 char   numpunct< char  >::do_decimal_point() const {return __decimal_point_;}
wchar_t numpunct<wchar_t>::do_decimal_point() const {return __decimal_point_;}

 char   numpunct< char  >::do_thousands_sep() const {return __thousands_sep_;}
wchar_t numpunct<wchar_t>::do_thousands_sep() const {return __thousands_sep_;}

string numpunct< char  >::do_grouping() const {return __grouping_;}
string numpunct<wchar_t>::do_grouping() const {return __grouping_;}

 string numpunct< char  >::do_truename() const {return "true";}
wstring numpunct<wchar_t>::do_truename() const {return L"true";}

 string numpunct< char  >::do_falsename() const {return "false";}
wstring numpunct<wchar_t>::do_falsename() const {return L"false";}

// numpunct_byname<char>

numpunct_byname<char>::numpunct_byname(const char* nm, size_t refs)
    : numpunct<char>(refs)
{
    __init(nm);
}

numpunct_byname<char>::numpunct_byname(const string& nm, size_t refs)
    : numpunct<char>(refs)
{
    __init(nm.c_str());
}

numpunct_byname<char>::~numpunct_byname()
{
}

void
numpunct_byname<char>::__init(const char* nm)
{
    if (strcmp(nm, "C") != 0)
    {
        __libcpp_unique_locale loc(nm);
        if (!loc)
            __throw_runtime_error("numpunct_byname<char>::numpunct_byname"
                                " failed to construct for " + string(nm));

        lconv* lc = __libcpp_localeconv_l(loc.get());
        __checked_string_to_char_convert(__decimal_point_, lc->decimal_point,
                                         loc.get());
        __checked_string_to_char_convert(__thousands_sep_, lc->thousands_sep,
                                         loc.get());
        __grouping_ = lc->grouping;
        // localization for truename and falsename is not available
    }
}

// numpunct_byname<wchar_t>

numpunct_byname<wchar_t>::numpunct_byname(const char* nm, size_t refs)
    : numpunct<wchar_t>(refs)
{
    __init(nm);
}

numpunct_byname<wchar_t>::numpunct_byname(const string& nm, size_t refs)
    : numpunct<wchar_t>(refs)
{
    __init(nm.c_str());
}

numpunct_byname<wchar_t>::~numpunct_byname()
{
}

void
numpunct_byname<wchar_t>::__init(const char* nm)
{
    if (strcmp(nm, "C") != 0)
    {
        __libcpp_unique_locale loc(nm);
        if (!loc)
            __throw_runtime_error("numpunct_byname<wchar_t>::numpunct_byname"
                                " failed to construct for " + string(nm));

        lconv* lc = __libcpp_localeconv_l(loc.get());
        __checked_string_to_wchar_convert(__decimal_point_, lc->decimal_point,
                                          loc.get());
        __checked_string_to_wchar_convert(__thousands_sep_, lc->thousands_sep,
                                          loc.get());
        __grouping_ = lc->grouping;
        // localization for truename and falsename is not available
    }
}

// num_get helpers

int
__num_get_base::__get_base(ios_base& iob)
{
    ios_base::fmtflags __basefield = iob.flags() & ios_base::basefield;
    if (__basefield == ios_base::oct)
        return 8;
    else if (__basefield == ios_base::hex)
        return 16;
    else if (__basefield == 0)
        return 0;
    return 10;
}

const char __num_get_base::__src[33] = "0123456789abcdefABCDEFxX+-pPiInN";

void
__check_grouping(const string& __grouping, unsigned* __g, unsigned* __g_end,
                 ios_base::iostate& __err)
{
//  if the grouping pattern is empty _or_ there are no grouping bits, then do nothing
//  we always have at least a single entry in [__g, __g_end); the end of the input sequence
	if (__grouping.size() != 0 && __g_end - __g > 1)
    {
        reverse(__g, __g_end);
        const char* __ig = __grouping.data();
        const char* __eg = __ig + __grouping.size();
        for (unsigned* __r = __g; __r < __g_end-1; ++__r)
        {
            if (0 < *__ig && *__ig < numeric_limits<char>::max())
            {
                if (static_cast<unsigned>(*__ig) != *__r)
                {
                    __err = ios_base::failbit;
                    return;
                }
            }
            if (__eg - __ig > 1)
                ++__ig;
        }
        if (0 < *__ig && *__ig < numeric_limits<char>::max())
        {
            if (static_cast<unsigned>(*__ig) < __g_end[-1] || __g_end[-1] == 0)
                __err = ios_base::failbit;
        }
    }
}

void
__num_put_base::__format_int(char* __fmtp, const char* __len, bool __signd,
                             ios_base::fmtflags __flags)
{
    if (__flags & ios_base::showpos)
        *__fmtp++ = '+';
    if (__flags & ios_base::showbase)
        *__fmtp++ = '#';
    while(*__len)
        *__fmtp++ = *__len++;
    if ((__flags & ios_base::basefield) == ios_base::oct)
        *__fmtp = 'o';
    else if ((__flags & ios_base::basefield) == ios_base::hex)
    {
        if (__flags & ios_base::uppercase)
            *__fmtp = 'X';
        else
            *__fmtp = 'x';
    }
    else if (__signd)
        *__fmtp = 'd';
    else
        *__fmtp = 'u';
}

bool
__num_put_base::__format_float(char* __fmtp, const char* __len,
                               ios_base::fmtflags __flags)
{
    bool specify_precision = true;
    if (__flags & ios_base::showpos)
        *__fmtp++ = '+';
    if (__flags & ios_base::showpoint)
        *__fmtp++ = '#';
    ios_base::fmtflags floatfield = __flags & ios_base::floatfield;
    bool uppercase = (__flags & ios_base::uppercase) != 0;
    if (floatfield == (ios_base::fixed | ios_base::scientific))
        specify_precision = false;
    else
    {
        *__fmtp++ = '.';
        *__fmtp++ = '*';
    }
    while(*__len)
        *__fmtp++ = *__len++;
    if (floatfield == ios_base::fixed)
    {
        if (uppercase)
            *__fmtp = 'F';
        else
            *__fmtp = 'f';
    }
    else if (floatfield == ios_base::scientific)
    {
        if (uppercase)
            *__fmtp = 'E';
        else
            *__fmtp = 'e';
    }
    else if (floatfield == (ios_base::fixed | ios_base::scientific))
    {
        if (uppercase)
            *__fmtp = 'A';
        else
            *__fmtp = 'a';
    }
    else
    {
        if (uppercase)
            *__fmtp = 'G';
        else
            *__fmtp = 'g';
    }
    return specify_precision;
}

char*
__num_put_base::__identify_padding(char* __nb, char* __ne,
                                   const ios_base& __iob)
{
    switch (__iob.flags() & ios_base::adjustfield)
    {
    case ios_base::internal:
        if (__nb[0] == '-' || __nb[0] == '+')
            return __nb+1;
        if (__ne - __nb >= 2 && __nb[0] == '0'
                            && (__nb[1] == 'x' || __nb[1] == 'X'))
            return __nb+2;
        break;
    case ios_base::left:
        return __ne;
    case ios_base::right:
    default:
        break;
    }
    return __nb;
}

void __do_nothing(void*) {}

void __throw_runtime_error(const char* msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw runtime_error(msg);
#else
    (void)msg;
    _VSTD::abort();
#endif
}

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS collate<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS collate<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS num_get<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS num_get<wchar_t>;

template struct _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __num_get<char>;
template struct _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __num_get<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS num_put<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS num_put<wchar_t>;

template struct _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __num_put<char>;
template struct _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __num_put<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS messages<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS messages<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS messages_byname<char>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS messages_byname<wchar_t>;

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS codecvt_byname<char, char, mbstate_t>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS codecvt_byname<wchar_t, char, mbstate_t>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS codecvt_byname<char16_t, char, mbstate_t>;
template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS codecvt_byname<char32_t, char, mbstate_t>;

_LIBCPP_END_NAMESPACE_STD
