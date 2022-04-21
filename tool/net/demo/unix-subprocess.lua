-- example of how to run the ls command
-- and pipe its output to the http user
local unix = require "unix"
function main()
   syscall = 'commandv'
   ls, errno = unix.commandv("ls")
   if ls then
      syscall = 'pipe'
      reader, writer, errno = unix.pipe()
      if reader then
         syscall = 'fork'
         child, errno = unix.fork()
         if child then
            if child == 0 then
               unix.close(1)
               unix.dup(writer)
               unix.close(writer)
               unix.close(reader)
               unix.execve(ls, {ls, "-Shal"})
               unix.exit(127)
            else
               unix.close(writer)
               SetStatus(200)
               SetHeader('Content-Type', 'text/plain')
               while true do
                  data = unix.read(reader)
                  if data ~= "" then
                     Write(data)
                  else
                     break
                  end
               end
               unix.close(reader)
               unix.wait(-1)
               return
            end
         end
      end
   end
   SetStatus(200)
   SetHeader('Content-Type', 'text/plain')
   Write(string.format('error %s calling %s()', unix.strerrno(errno), syscall))
end
main()
