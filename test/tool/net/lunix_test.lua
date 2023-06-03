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

gotsigusr1 = false
tmpdir = "%s/o/tmp/lunix_test.%d" % {os.getenv('TMPDIR'), unix.getpid()}

function string.starts(String,Start)
   return string.sub(String,1,string.len(Start))==Start
end

function OnSigUsr1(sig)
   gotsigusr1 = true
end

function UnixTest()

   -- strsignal
   assert(unix.strsignal(9) == "SIGKILL")
   assert(unix.strsignal(unix.SIGKILL) == "SIGKILL")

   -- gmtime
   year,mon,mday,hour,min,sec,gmtoffsec,wday,yday,dst,zone = assert(unix.gmtime(1657297063))
   assert(year == 2022)
   assert(mon == 7)
   assert(mday == 8)
   assert(hour == 16)
   assert(min == 17)
   assert(sec == 43)
   assert(gmtoffsec == 0)
   assert(wday == 5)
   assert(yday == 188)
   assert(dst == 0)
   assert(zone == "GMT")

   -- dup
   -- 1. duplicate stderr as lowest available fd
   -- 1. close the newly assigned file descriptor
   fd = assert(unix.dup(2))
   assert(unix.close(fd))

   -- dup2
   -- 1. duplicate stderr as fd 10
   -- 1. close the new file descriptor
   assert(assert(unix.dup(2, 10)) == 10)
   assert(unix.close(10))

   -- fork
   -- basic subprocess creation
   if assert(unix.fork()) == 0 then
      assert(unix.pledge(""))
      unix.exit(42)
   end
   pid, ws = assert(unix.wait())
   assert(unix.WIFEXITED(ws))
   assert(unix.WEXITSTATUS(ws) == 42)

   -- pledge
   -- 1. fork off a process
   -- 2. sandbox the process
   -- 3. then violate its security
   if unix.pledge(nil, nil) then
       reader, writer = assert(unix.pipe())
       if assert(unix.fork()) == 0 then
           assert(unix.dup(writer, 2))
           assert(unix.pledge("stdio"))
           unix.socket()
           unix.exit(0)
       end
       unix.close(writer)
       unix.close(reader)
       pid, ws = assert(unix.wait())
       assert(unix.WIFSIGNALED(ws))
       assert(unix.WTERMSIG(ws) == unix.SIGSYS or  -- Linux
              unix.WTERMSIG(ws) == unix.SIGABRT)   -- OpenBSD
   end

   -- sigaction
   -- 1. install a signal handler for USR1
   -- 2. block USR1
   -- 3. trigger USR1 signal [it gets enqueued]
   -- 4. pause() w/ atomic unblocking of USR1 [now it gets delivered!]
   -- 5. restore old signal mask
   -- 6. restore old sig handler
   oldhand, oldflags, oldmask = assert(unix.sigaction(unix.SIGUSR1, OnSigUsr1))
   oldmask = assert(unix.sigprocmask(unix.SIG_BLOCK, unix.Sigset(unix.SIGUSR1)))
   assert(unix.raise(unix.SIGUSR1))
   assert(not gotsigusr1)
   ok, err = unix.sigsuspend(oldmask)
   assert(not ok)
   assert(err:errno() == unix.EINTR)
   assert(gotsigusr1)
   assert(unix.sigprocmask(unix.SIG_SETMASK, oldmask))
   assert(unix.sigaction(unix.SIGUSR1, oldhand, oldflags, oldmask))

   -- open
   -- 1. create file
   -- 2. fill it up
   -- 3. inspect it
   -- 4. mess with it
   fd = assert(unix.open("%s/foo" % {tmpdir}, unix.O_RDWR | unix.O_CREAT | unix.O_TRUNC, 0600))
   assert(assert(unix.fstat(fd)):size() == 0)
   assert(unix.ftruncate(fd, 8192))
   assert(assert(unix.fstat(fd)):size() == 8192)
   assert(unix.write(fd, "hello"))
   assert(unix.lseek(fd, 4096))
   assert(unix.write(fd, "poke"))
   assert(unix.lseek(fd, 8192-4))
   assert(unix.write(fd, "poke"))
   st = assert(unix.fstat(fd))
   assert(st:size() == 8192)
   assert(st:blocks() == 8192/512)
   assert((st:mode() & 0777) == 0600)
   assert(st:uid() == unix.getuid())
   assert(st:gid() == unix.getgid())
   assert(unix.write(fd, "bear", 4))
   assert(unix.read(fd, 10, 0) == "hellbear\x00\x00")
   assert(unix.close(fd))
   fd = assert(unix.open("%s/foo" % {tmpdir}))
   assert(unix.lseek(fd, 4))
   assert(unix.read(fd, 4) == "bear")
   assert(unix.close(fd))
   fd = assert(unix.open("%s/foo" % {tmpdir}, unix.O_RDWR))
   assert(unix.write(fd, "bear"))
   assert(unix.close(fd))
   fd = assert(unix.open("%s/foo" % {tmpdir}))
   assert(unix.read(fd, 8) == "bearbear")
   assert(unix.close(fd))

   -- getdents
   t = {}
   for name, kind, ino, off in assert(unix.opendir(tmpdir)) do
      table.insert(t, name)
   end
   table.sort(t)
   assert(EncodeLua(t) == '{".", "..", "foo"}');

end

function main()
   assert(unix.makedirs(tmpdir))
   unix.unveil(tmpdir, "rwc")
   unix.unveil(nil, nil)
   assert(unix.pledge("stdio rpath wpath cpath proc"))
   ok, err = pcall(UnixTest)
   if ok then
      assert(unix.rmrf(tmpdir))
   else
      print(err)
      error('UnixTest failed (%s)' % {tmpdir})
   end
end

main()
