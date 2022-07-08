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

-- dup()+close()
fd = assert(unix.dup(2))
assert(unix.close(fd))

-- dup2()+close()
assert(assert(unix.dup(2, 10)) == 10)
assert(unix.close(10))

-- fork()+exit()
if assert(unix.fork()) == 0 then
   unix.exit(42)
end
pid, ws = assert(unix.wait())
assert(unix.WIFEXITED(ws))
assert(unix.WEXITSTATUS(ws) == 42)

-- pledge()
if GetHostOs() == "LINUX" then
   if assert(unix.fork()) == 0 then
      assert(unix.pledge("stdio"))
      _, err = unix.socket()
      assert(err:errno() == unix.EPERM)
      unix.exit(0)
   end
   pid, ws = assert(unix.wait())
   assert(unix.WIFEXITED(ws))
   assert(unix.WEXITSTATUS(ws) == 0)
elseif GetHostOs() == "OPENBSD" then
   if assert(unix.fork()) == 0 then
      assert(unix.pledge("stdio"))
      unix.socket()
      unix.exit(1)
   end
   pid, ws = assert(unix.wait())
   assert(unix.WIFSIGNALED(ws))
   assert(unix.WTERMSIG(ws) == unix.SIGABRT)
end
