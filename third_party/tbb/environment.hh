// clang-format off
/*
    Copyright (c) 2018-2022 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef __TBB_tbb_environment_H
#define __TBB_tbb_environment_H

#include "third_party/libcxx/cstdlib"
#include "third_party/libcxx/cstring"
#include "third_party/libcxx/cerrno"
#include "third_party/libcxx/cctype"

namespace tbb {
namespace detail {
namespace r1 {

#if __TBB_WIN8UI_SUPPORT
static inline bool GetBoolEnvironmentVariable( const char * ) {
    return false;
}

static inline long GetIntegralEnvironmentVariable( const char * ) {
    return -1;
}
#else  /* __TBB_WIN8UI_SUPPORT */
static inline bool GetBoolEnvironmentVariable( const char * name ) {
    if ( const char* s = std::getenv(name) ) {
        // The result is defined as true only if the environment variable contains
        // no characters except one '1' character and an arbitrary number of spaces
        // (including the absence of spaces).
        size_t index = std::strspn(s, " ");
        if (s[index] != '1') return false;
        index++;
        // Memory access after incrementing is safe, since the getenv() returns a
        // null-terminated string, and even if the character getting by index is '1',
        // and this character is the end of string, after incrementing we will get
        // an index of character, that contains '\0'
        index += std::strspn(&s[index], " ");
        return !s[index];
    }
    return false;
}

static inline long GetIntegralEnvironmentVariable( const char * name ) {
    if ( const char* s = std::getenv(name) ) {
        char* end = nullptr;
        errno = 0;
        long value = std::strtol(s, &end, 10);

        // We have exceeded the range, value is negative or string is incovertable
        if ( errno == ERANGE || value < 0 || end==s ) {
            return -1;
        }
        for ( ; *end != '\0'; end++ ) {
            if ( !std::isspace(*end) ) {
                return -1;
            }
        }
        return value;
    }
    return -1;
}
#endif /* __TBB_WIN8UI_SUPPORT */

} // namespace r1
} // namespace detail
} // namespace tbb

#endif // __TBB_tbb_environment_H
