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

-- let's use futexes to implement a scalable mutex in lua
--
-- this example is designed to be the kind of thing that would be
-- extremely expensive if we were using spin locks, because we'll
-- have a lot of processes that wait a long time for something to
-- happen. futexes solve that by asking the kernel to help us out
-- and let the waiting processes sleep until the actual event.
--
-- here we have fifty processes, waiting one just one process, to
-- to finish a long-running task. if we used spin locks, then the
-- waiter procesess would eat up all the cpu. if you benchmark it
-- then be sure to note that WALL TIME will be the same, it's the
-- CPU USER TIME that gets pwnd.
--
-- uses    67 ms cpu time with futexes
-- uses 5,000 ms cpu time with spinlocks
millis = 300
waiters = 100

words = 2
mem = unix.mapshared(words * 8)
LOCK = 0     -- word index of our lock
RESULT = 1   -- word index of our result

-- From Futexes Are Tricky Version 1.1 § Mutex, Take 3;
-- Ulrich Drepper, Red Hat Incorporated, June 27, 2004.
function Lock()
    local ok, old = mem:cmpxchg(LOCK, 0, 1)
    if not ok then
        if old == 1 then
            old = mem:xchg(LOCK, 2)
        end
        while old > 0 do
            mem:wait(LOCK, 2)
            old = mem:xchg(LOCK, 2)
        end
    end
end
function Unlock()
    local old = mem:fetch_add(LOCK, -1)
    if old == 2 then
        mem:store(LOCK, 0)
        mem:wake(LOCK, 1)
    end
end

-- -- try it out with spin locks instead
-- function Lock()
--     while mem:xchg(LOCK, 1) == 1 do
--     end
-- end
-- function Unlock()
--     mem:store(LOCK, 0)
-- end

function Worker()
    assert(unix.nanosleep(math.floor(millis / 1000),
                          millis % 1000 * 1000 * 1000))
    mem:store(RESULT, 123)
    Unlock()
end

function Waiter()
    Lock()
    assert(mem:load(RESULT) == 123)
    Unlock()
end

Lock()

for i = 1,waiters do
    pid = assert(unix.fork())
    if pid == 0 then
        Waiter()
        unix.exit(0)
    end
end

Worker()

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
