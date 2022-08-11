-- Copyright 2022 Justine Alexandra Roberts Tunney
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

assert("/usr/lib" == path.dirname("/usr/lib/foo.bar"))
assert("/usr" == path.dirname("/usr/lib"))
assert("usr" == path.dirname("usr/lib"))
assert("/" == path.dirname("/usr//"))
assert("/" == path.dirname("/usr/"))
assert("." == path.dirname("usr"))
assert("/" == path.dirname("/"))
assert("." == path.dirname("."))
assert("." == path.dirname(".."))
assert("." == path.dirname(""))
assert("." == path.dirname(nil))

--------------------------------------------------------------------------------

assert("lib" == path.basename("/usr/lib"))
assert("lib" == path.basename("usr/lib"))
assert("usr" == path.basename("/usr/"))
assert("usr" == path.basename("/usr"))
assert("/" == path.basename("/"))
assert("." == path.basename("."))
assert(".." == path.basename(".."))
assert("." == path.basename(""))
assert("foo" == path.basename("foo/"))
assert("foo" == path.basename("foo//"))
assert("/" == path.basename("///"))
assert("0" == path.basename(0))
assert("." == path.basename(nil))

--------------------------------------------------------------------------------

assert("a" == path.join("a"))
assert("a/b/c" == path.join("a", "b", "c"))
assert("/c" == path.join("a", "b", "/c"))
assert("/b/c" == path.join("a", "/b", "c"))
assert("/c" == path.join("a", "/b", "/c"))
assert("./." == path.join(".", "."))
assert("./.." == path.join(".", ".."))
assert("../." == path.join("..", "."))
assert("./a" == path.join(".", "a"))
assert("c//c" == path.join("c//", "c"))
assert("a/." == path.join("a", "."))
assert("a/b" == path.join("a", "b"))
assert("a/b" == path.join("a/", "b"))
assert("a/b/" == path.join("a", "b/"))
assert("/b" == path.join("a", "/b"))
assert("./b" == path.join(".", "b"))
assert("a" * 3000 .."/123/".. "b" * 3000 .. "/123" == path.join("a" * 3000, 123, "b" * 3000, 123))
assert("1/2/3" == path.join(1, 2, 3))
assert(nil == path.join(nil))
assert(nil == path.join(nil, nil))
assert("a" == path.join("a", nil))
assert("a" == path.join(nil, "a"))
assert("a/a" == path.join("a", nil, "a"))
assert("a/a" == path.join("a/", nil, "a"))
assert("" == path.join(""))
assert("" == path.join("", ""))
assert("b" == path.join("", "b"))
assert("a" == path.join("", "a"))
assert("a/" == path.join("a", ""))
assert("a/b" == path.join("a", "", "b"))
assert("a/b" == path.join("a", "", "", "b"))
assert("a/b/" == path.join("a", "", "", "b", ""))
