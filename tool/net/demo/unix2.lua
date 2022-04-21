-- example of how to run the ls command
-- and pipe its output to the http user
local unix = require "unix"
ls = unix.commandv("ls")
reader, writer = unix.pipe()
if unix.fork() == 0 then
   unix.close(1)
   unix.dup(writer)
   unix.close(writer)
   unix.close(reader)
   unix.execve(ls, {ls, "-Shal"})
   unix.exit(127)
else
   unix.close(writer)
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
end
