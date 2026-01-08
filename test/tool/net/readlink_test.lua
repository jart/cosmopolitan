-- Copyright 2026 Justine Alexandra Roberts Tunney
--
-- Permission to use, copy, modify, and/or distribute this software for
-- any purpose with or without fee is hereby granted, provided that the
-- above copyright notice and this permission notice appear in all copies.
--
-- THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
-- WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
-- WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
-- AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
-- DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
-- PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
-- TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
-- PERFORMANCE OF THIS SOFTWARE.

-- test: unix.readlink() works with relative paths
--
-- This test verifies that unix.readlink() can read symbolic links
-- using relative paths, not just absolute paths.

local unix = require("cosmo.unix")
local path = require("cosmo.path")

-- create a symlink in a relative directory
local test_dir = "o/tmp/readlink_test"
unix.makedirs(test_dir)
local link_path = path.join(test_dir, "testlink")
unix.unlink(link_path)
unix.symlink("target_file", link_path)

-- test 1: reading with relative path should work
local rel_result, rel_err = unix.readlink(link_path)
assert(rel_result == "target_file",
  "relative path readlink should work, got: " .. tostring(rel_err or rel_result))

-- test 2: reading with absolute path should also work
local abs_path = path.join(unix.getcwd(), link_path)
local abs_result, abs_err = unix.readlink(abs_path)
assert(abs_result == "target_file",
  "absolute path readlink should work, got: " .. tostring(abs_err or abs_result))

-- test 3: optional bufsiz parameter should work
local sized_result, sized_err = unix.readlink(link_path, 1024)
assert(sized_result == "target_file",
  "readlink with bufsiz should work, got: " .. tostring(sized_err or sized_result))

-- test 4: small buffer should truncate
local small_result, small_err = unix.readlink(link_path, 6)
assert(small_result == "target",
  "readlink with small bufsiz should truncate, got: " .. tostring(small_err or small_result))

-- cleanup
unix.unlink(link_path)
unix.rmdir(test_dir)
unix.rmdir("o/tmp")

print("unix.readlink() relative path tests passed")
