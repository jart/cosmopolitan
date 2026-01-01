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

-- Test basic setenv
assert(unix.setenv("TEST_VAR", "test_value"))
assert(os.getenv("TEST_VAR") == "test_value")

-- Test overwrite with default (should overwrite)
assert(unix.setenv("TEST_VAR", "new_value"))
assert(os.getenv("TEST_VAR") == "new_value")

-- Test overwrite = true
assert(unix.setenv("TEST_VAR", "overwritten_value", true))
assert(os.getenv("TEST_VAR") == "overwritten_value")

-- Test overwrite = false (should not overwrite)
assert(unix.setenv("TEST_VAR", "should_not_change", false))
assert(os.getenv("TEST_VAR") == "overwritten_value")

-- Test setting a new variable with overwrite = false
assert(unix.setenv("ANOTHER_VAR", "another_value", false))
assert(os.getenv("ANOTHER_VAR") == "another_value")

-- Test with empty value
assert(unix.setenv("EMPTY_VAR", ""))
assert(os.getenv("EMPTY_VAR") == "")

print("All setenv tests passed!")
