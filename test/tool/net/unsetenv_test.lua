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

-- Set a test variable first
assert(unix.setenv("TEST_UNSET_VAR", "test_value"))
assert(os.getenv("TEST_UNSET_VAR") == "test_value")

-- Test basic unsetenv
assert(unix.unsetenv("TEST_UNSET_VAR"))
assert(os.getenv("TEST_UNSET_VAR") == nil)

-- Test unsetting a non-existent variable (should succeed)
assert(unix.unsetenv("NONEXISTENT_VAR"))

-- Test unsetting multiple variables
assert(unix.setenv("VAR1", "value1"))
assert(unix.setenv("VAR2", "value2"))
assert(os.getenv("VAR1") == "value1")
assert(os.getenv("VAR2") == "value2")

assert(unix.unsetenv("VAR1"))
assert(os.getenv("VAR1") == nil)
assert(os.getenv("VAR2") == "value2")

assert(unix.unsetenv("VAR2"))
assert(os.getenv("VAR2") == nil)

print("All unsetenv tests passed!")
