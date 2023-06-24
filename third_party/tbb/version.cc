// clang-format off
/*
    Copyright (c) 2020-2021 Intel Corporation

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

#include "third_party/tbb/version.hh"

extern "C" int TBB_runtime_interface_version() {
    return TBB_INTERFACE_VERSION;
}

extern "C" const char* TBB_runtime_version() {
    static const char version_str[] = TBB_VERSION_STRING;
    return version_str;
}
