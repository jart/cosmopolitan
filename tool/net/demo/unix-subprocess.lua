-- example of how to run the ls command
-- and pipe its output to the http user
local unix = require "unix"
function main()
   if GetHostOs() == 'WINDOWS' then
      cmd = 'dir'
   else
      cmd = 'ls'
   end
   syscall = 'commandv'
   ls, errno = unix.commandv(cmd)
   if ls then
      syscall = 'pipe'
      reader, writer, errno = unix.pipe()
      if reader then
         -- oldint = unix.sigaction(unix.SIGINT, unix.SIG_IGN)
         -- oldquit = unix.sigaction(unix.SIGQUIT, unix.SIG_IGN)
         -- oldmask = unix.sigprocmask(unix.SIG_BLOCK, unix.SIGCHLD)
         syscall = 'fork'
         child, errno = unix.fork()
         if child then
            if child == 0 then
               unix.close(1)
               unix.dup(writer)
               unix.close(writer)
               unix.close(reader)
               -- unix.sigaction(unix.SIGINT, oldint)
               -- unix.sigaction(unix.SIGQUIT, oldquit)
               -- unix.sigprocmask(unix.SIG_SETMASK, oldmask)
               unix.execve(ls, {ls, '-Shal'})
               unix.exit(127)
            else
               unix.close(writer)
               SetStatus(200)
               SetHeader('Content-Type', 'text/plain')
               while true do
                  data, errno = unix.read(reader)
                  if data then
                     if data ~= '' then
                        Write(data)
                     else
                        break
                     end
                  elseif errno ~= unix.EINTR then
                     Log(kLogWarn, 'read() failed: %s' % {unix.strerror(errno)})
                     break
                  end
               end
               unix.close(reader)
               unix.wait(-1)
               -- unix.sigaction(unix.SIGINT, oldint)
               -- unix.sigaction(unix.SIGQUIT, oldquit)
               -- unix.sigprocmask(unix.SIG_SETMASK, oldmask)
               return
            end
         end
      end
   end
   SetStatus(200)
   SetHeader('Content-Type', 'text/plain')
   Write('error %s calling %s()' % {unix.strerrno(errno), syscall})
end
main()
