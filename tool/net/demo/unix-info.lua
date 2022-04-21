local unix = require 'unix'

Write('<!doctype html>\r\n')
Write('<title>redbean</title>\r\n')
Write('<h3>UNIX Information Demo</h3>\r\n')

Write('<dl>\r\n')
Write('<dt>getuid()\r\n')
Write(string.format('<dd>%d\r\n', unix.getuid()))
Write('<dt>getgid()\r\n')
Write(string.format('<dd>%d\r\n', unix.getgid()))
Write('<dt>getpid()\r\n')
Write(string.format('<dd>%d\r\n', unix.getpid()))
Write('<dt>getppid()\r\n')
Write(string.format('<dd>%d\r\n', unix.getppid()))
Write('<dt>getpgrp()\r\n')
Write(string.format('<dd>%d\r\n', unix.getpgrp()))

Write('<dt>getsid(0)\r\n')
sid, errno = unix.getsid(0)
if sid then
   Write(string.format('<dd>%d\r\n', sid))
else
   Write(string.format('<dd>%s\r\n', EscapeHtml(unix.strerrno(errno))))
end

Write('<dt>gethostname()\r\n')
Write(string.format('<dd>%s\r\n', EscapeHtml(unix.gethostname())))
Write('<dt>getcwd()\r\n')
Write(string.format('<dd>%s\r\n', EscapeHtml(unix.getcwd())))

function PrintResourceLimit(name, id)
   soft, hard, errno = unix.getrlimit(id)
   Write(string.format('<dt>getrlimit(%s)\r\n', name))
   if soft then
      Write('<dd>')
      Write('soft ')
      if soft == -1 then
         Write('∞')
      else
         Write(string.format('%d', soft))
      end
      Write('<br>\r\n')
      Write('hard ')
      if hard == -1 then
         Write('∞')
      else
         Write(string.format('%d', hard))
      end
      Write('\r\n')
   else
      Write(string.format('<dd>%s\r\n', EscapeHtml(unix.strerrno(errno))))
   end
end
PrintResourceLimit('RLIMIT_AS', unix.RLIMIT_AS)
PrintResourceLimit('RLIMIT_RSS', unix.RLIMIT_RSS)
PrintResourceLimit('RLIMIT_CPU', unix.RLIMIT_CPU)
PrintResourceLimit('RLIMIT_FSIZE', unix.RLIMIT_FSIZE)
PrintResourceLimit('RLIMIT_NPROC', unix.RLIMIT_NPROC)
PrintResourceLimit('RLIMIT_NOFILE', unix.RLIMIT_NOFILE)

Write('<dt>siocgifconf()\r\n')
Write('<dd>\r\n')
ifs, errno = unix.siocgifconf()
if ifs then
   for i = 1,#ifs do
      if ifs[i].netmask ~= 0 then
         cidr = 32 - Bsf(ifs[i].netmask)
      else
         cidr = 0
      end
      Write(string.format('%s %s/%d<br>\r\n',
                          EscapeHtml(ifs[i].name),
                          FormatIp(ifs[i].ip),
                          cidr))
   end
else
   Write(string.format('%s\r\n', EscapeHtml(unix.strerrno(errno))))
end
Write('</dl>\r\n')
