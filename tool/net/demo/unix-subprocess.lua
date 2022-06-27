-- example of how to run the ls command
-- and pipe its output to the http user
local unix = require "unix"
function main()
   if GetHostOs() == 'WINDOWS' then
      cmd = 'dir'
   else
      cmd = 'ls'
   end
   ls = assert(unix.commandv(cmd))
   reader, writer = assert(unix.pipe(unix.O_CLOEXEC))
   oldint = assert(unix.sigaction(unix.SIGINT, unix.SIG_IGN))
   oldquit = assert(unix.sigaction(unix.SIGQUIT, unix.SIG_IGN))
   oldmask = assert(unix.sigprocmask(unix.SIG_BLOCK, unix.Sigset(unix.SIGCHLD)))
   child = assert(unix.fork())
   if child == 0 then
      unix.close(0)
      unix.open("/dev/null", unix.O_RDONLY)
      unix.close(1)
      unix.dup(writer)
      unix.close(2)
      unix.open("/dev/null", unix.O_RDONLY)
      unix.sigaction(unix.SIGINT, oldint)
      unix.sigaction(unix.SIGQUIT, oldquit)
      unix.sigprocmask(unix.SIG_SETMASK, oldmask)
      unix.execve(ls, {ls, '-Shal'})
      unix.exit(127)
   else
      unix.close(writer)
      SetStatus(200)
      SetHeader('Content-Type', 'text/plain')
      while true do
         data, err = unix.read(reader)
         if data then
            if data ~= '' then
               Write(data)
            else
               break
            end
         elseif err:errno() ~= unix.EINTR then
            Log(kLogWarn, 'read() failed: %s' % {tostring(err)})
            break
         end
      end
      unix.close(reader)
      Log(kLogWarn, 'wait() begin')
      unix.wait(-1)
      Log(kLogWarn, 'wait() end')
      unix.sigaction(unix.SIGINT, oldint)
      unix.sigaction(unix.SIGQUIT, oldquit)
      unix.sigprocmask(unix.SIG_SETMASK, oldmask)
      return
   end
end
main()
