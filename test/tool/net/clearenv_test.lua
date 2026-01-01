-- Copyright 2024 Will Maier
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

assert(unix.pledge("stdio"))

-- Set some test variables
assert(unix.setenv("TEST_VAR1", "value1"))
assert(unix.setenv("TEST_VAR2", "value2"))
assert(unix.setenv("TEST_VAR3", "value3"))

-- Verify they're set
assert(os.getenv("TEST_VAR1") == "value1")
assert(os.getenv("TEST_VAR2") == "value2")
assert(os.getenv("TEST_VAR3") == "value3")

-- Clear all environment variables
assert(unix.clearenv())

-- Verify they're all gone
assert(os.getenv("TEST_VAR1") == nil)
assert(os.getenv("TEST_VAR2") == nil)
assert(os.getenv("TEST_VAR3") == nil)

-- Check that environ is empty
local env = unix.environ()
assert(#env == 0)

-- Test that we can still set variables after clearing
assert(unix.setenv("NEW_VAR", "new_value"))
assert(os.getenv("NEW_VAR") == "new_value")

print("All clearenv tests passed!")
