-- Copyright 2023 Justine Alexandra Roberts Tunney
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

local sqlite3 = require "lsqlite3"
local db = assert(sqlite3.open("file:/memdb1?vfs=memdb&mode=ro",
  sqlite3.OPEN_URI + sqlite3.OPEN_READWRITE + sqlite3.OPEN_CREATE))
assert(db:readonly() == true)
db = sqlite3.open("file:/memdb1?vfs=memdb",
  sqlite3.OPEN_URI + sqlite3.OPEN_READWRITE + sqlite3.OPEN_CREATE)
assert(db:readonly() == false)
assert(db:readonly("main") == false)
assert(db:readonly("foo") == nil)

assert(db:exec("create table foo(a)") == 0)

local st = assert(db:prepare("select * from foo"))
assert(st:readonly() == true)
st = assert(db:prepare("insert into foo (a) values (1)"))
assert(st:readonly() == false)
