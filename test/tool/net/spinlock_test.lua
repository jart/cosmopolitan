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
-- let's use atomics to implement a spin lock in lua

LOCK = 0     -- word index of our lock
COUNTER = 1  -- word index of our number

function Lock()
    while mem:xchg(LOCK, 1) == 1 do
    end
end

function Unlock()
    mem:store(LOCK, 0)
end

function Worker()
    local x
    for i = 1,iterations do
        Lock()
        x = mem:load(COUNTER)
        x = x + 1
        mem:store(COUNTER, x)
        Unlock()
    end
end

mem:store(LOCK, 0)
mem:store(COUNTER, 0)
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

assert(mem:load(COUNTER) == processes * iterations)
