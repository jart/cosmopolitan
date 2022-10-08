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

assert(unix.pledge("stdio proc"))

words = 2
processes = 8
iterations = 10000

mem = unix.mapshared(words * 8)

--------------------------------------------------------------------------------
-- test shared memory string reading and writing

mem:write('hello')
assert(mem:read() == 'hello')
mem:write('hi')
assert(mem:read() == 'hi')
assert(mem:read(0, 5) == 'hi\0lo')
mem:write(0, 'H', 1)
assert(mem:read(0, 5) == 'Hi\0lo')
assert(mem:read(1, 1) == 'i')

--------------------------------------------------------------------------------
-- test shared memory locking primitives

mem:store(0, 0)
assert(mem:xchg(0, 1) == 0)
assert(mem:xchg(0, 2) == 1)

mem:store(0, 0)
ok, old = mem:cmpxchg(0, 0, 1)
assert(ok and old == 0)
ok, old = mem:cmpxchg(0, 666, 777)
assert(not ok and old == 1)
assert(mem:fetch_add(0, 3) == 1)
assert(mem:load(0) ==  0b00000100)
assert(mem:fetch_xor(0,0b00000110) == 0b00000100)
assert(mem:load(0) ==  0b00000010)
assert(mem:fetch_and(0,0b00000110) == 0b00000010)
assert(mem:load(0) ==  0b00000010)
assert(mem:fetch_or(0, 0b00000110) == 0b00000010)
assert(mem:load(0) ==  0b00000110)

--------------------------------------------------------------------------------
-- test atomic addition across concurrent processes

function Worker()
    for i = 1,iterations do
        mem:fetch_add(0, 1)
    end
end

mem:store(0, 0)
for i = 1,processes do
    pid = assert(unix.fork())
    if pid == 0 then
        Worker()
        unix.exit(0)
    end
end
while true do
    rc, ws = unix.wait(0)
    if not rc then
        assert(ws:errno() == unix.ECHILD)
        break
    end
    if unix.WIFEXITED(ws) then
        if unix.WEXITSTATUS(ws) ~= 0 then
            print('process %d exited with %s' % {rc, unix.WEXITSTATUS(ws)})
            unix.exit(1)
        end
    else
        print('process %d terminated with %s' % {rc, unix.WTERMSIG(ws)})
        unix.exit(1)
    end
end

assert(mem:load(0) == processes * iterations)
