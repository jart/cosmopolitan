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

tmpdir = "%s/o/tmp/lunix_test.%d" % {os.getenv('TMPDIR'), unix.getpid()}

local function Path(name)
   return tmpdir .. '/' .. name
end

local function SlurpTest()

   data = 'abc123' * 5000
   assert(Barf(Path('foo'), data))
   assert(assert(Slurp(Path('foo'))) == data)
   assert(assert(Slurp(Path('foo'), 2, 3)) == 'bc')
   assert(assert(Slurp(Path('foo'), 2)) == data:sub(2))
   assert(assert(Slurp(Path('foo'), 2, 1)) == data:sub(2, 1))
   assert(assert(Slurp(Path('foo'), 2, 2)) == data:sub(2, 2))
   assert(assert(Slurp(Path('foo'), 1, 2)) == data:sub(1, 2))
   assert(assert(Slurp(Path('foo'), -3, -1)) == data:sub(-3, -1))

   assert(Barf(Path('foo'), 'XX', 0, 0, 3))
   assert(assert(Slurp(Path('foo'), 1, 6)) == 'abXX23')

end

local function main()
   assert(unix.makedirs(tmpdir))
   unix.unveil(tmpdir, "rwc")
   unix.unveil(nil, nil)
   assert(unix.pledge("stdio rpath wpath cpath"))
   ok, err = pcall(SlurpTest)
   if ok then
      assert(unix.rmrf(tmpdir))
   else
      print(err)
      error('SlurpTest failed (%s)' % {tmpdir})
   end
end

main()
