// clang-format off
/* Copyright 2016 Google Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License. */

#include "libc/thread/nsync_cpp.h"
#include "libc/thread/platform.h"
#include "libc/thread/compiler.h"
#include "libc/thread/cputype.h"
#include "libc/thread/nsync_time.h"

NSYNC_CPP_START_

nsync_time nsync_time_ms (unsigned ms) {
        unsigned s = ms / 1000;
	return (nsync_time_s_ns (s, 1000 * 1000 * (ms % 1000)));
}

nsync_time nsync_time_us (unsigned us) {
        unsigned s = us / (1000 * 1000);
	return (nsync_time_s_ns (s, 1000 * (us % (1000 * 1000))));
}

NSYNC_CPP_END_
