local unix = require 'unix'

Write('<!doctype html>\r\n')
Write('<title>redbean</title>\r\n')
Write('<h3>UNIX Information Demo</h3>\r\n')
Write('<style>dt { margin: .5em 0; font-style:italic; }</style>\r\n')

Write('<dl>\r\n')
Write('<dt>unix.getuid()\r\n')
Write('<dd>%d\r\n' % {unix.getuid()})
Write('<dt>unix.getgid()\r\n')
Write('<dd>%d\r\n' % {unix.getgid()})
Write('<dt>unix.getpid()\r\n')
Write('<dd>%d\r\n' % {unix.getpid()})
Write('<dt>unix.getppid()\r\n')
Write('<dd>%d\r\n' % {unix.getppid()})
Write('<dt>unix.getpgrp()\r\n')
Write('<dd>%d\r\n' % {unix.getpgrp()})
Write('<dt>unix.umask()\r\n')
mask = unix.umask(027)
unix.umask(mask)
Write('<dd>%.4o\r\n' % {mask})

Write('<dt>unix.getsid(0)\r\n')
sid, err = unix.getsid(0)
if sid then
   Write('<dd>%d\r\n' % {sid})
else
   Write('<dd>%s\r\n' % {err})
end

Write('<dt>unix.gethostname()\r\n')
Write('<dd>%s\r\n' % {EscapeHtml(assert(unix.gethostname()))})
Write('<dt>unix.getcwd()\r\n')
Write('<dd>%s\r\n' % {EscapeHtml(assert(unix.getcwd()))})

function PrintResourceLimit(name, id)
   soft, hard = unix.getrlimit(id)
   Write('<dt>getrlimit(%s)\r\n' % {name})
   if soft then
      Write('<dd>')
      Write('soft ')
      if soft == -1 then
         Write('∞')
      else
         Write('%d' % {soft})
      end
      Write('<br>\r\n')
      Write('hard ')
      if hard == -1 then
         Write('∞')
      else
         Write('%d' % {hard})
      end
      Write('\r\n')
   else
      Write('<dd>%s\r\n' % {EscapeHtml(tostring(hard))})
   end
end
PrintResourceLimit('RLIMIT_AS', unix.RLIMIT_AS)
PrintResourceLimit('RLIMIT_RSS', unix.RLIMIT_RSS)
PrintResourceLimit('RLIMIT_CPU', unix.RLIMIT_CPU)
PrintResourceLimit('RLIMIT_FSIZE', unix.RLIMIT_FSIZE)
PrintResourceLimit('RLIMIT_NPROC', unix.RLIMIT_NPROC)
PrintResourceLimit('RLIMIT_NOFILE', unix.RLIMIT_NOFILE)

Write('<dt>unix.siocgifconf()\r\n')
Write('<dd>\r\n')
ifs, err = unix.siocgifconf()
if ifs then
   for i = 1,#ifs do
      if ifs[i].netmask ~= 0 then
         cidr = 32 - Bsf(ifs[i].netmask)
      else
         cidr = 0
      end
      Write('%s %s/%d<br>\r\n' % {EscapeHtml(ifs[i].name), FormatIp(ifs[i].ip), cidr})
   end
else
   Write('%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_DEBUG)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_DEBUG)\r\n')
if enabled then -- is nil on error
   Write('<dd>%d\r\n' % {enabled}) -- should be 0 or 1
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_ACCEPTCONN)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_ACCEPTCONN)\r\n')
if enabled then -- is nil on error
   Write('<dd>%d\r\n' % {enabled}) -- should be 0 or 1
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_REUSEADDR)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_REUSEADDR)\r\n')
if enabled then -- is nil on error
   Write('<dd>%d\r\n' % {enabled}) -- should be 0 or 1
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_REUSEPORT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_REUSEPORT)\r\n')
if enabled then -- is nil on error
   Write('<dd>%d\r\n' % {enabled}) -- should be 0 or 1
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_KEEPALIVE)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_KEEPALIVE)\r\n')
if enabled then -- is nil on error
   Write('<dd>%s\r\n' % {enabled}) -- should be 0 or 1
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_NODELAY)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_NODELAY)\r\n')
if enabled then -- is nil on error
   Write('<dd>%s\r\n' % {enabled}) -- should be 0 or 1
else
   Write('<dd>%s\r\n' % {err})
end

secs, nanos = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVTIMEO)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVTIMEO)\r\n')
if secs then -- is nil on error
   Write('<dd>%d seconds + %d nanoseconds\r\n' % {secs, nanos})
else
   err = nanos
   Write('<dd>%s\r\n' % {err})
end

secs, nanos = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDTIMEO)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDTIMEO)\r\n')
if secs then -- is nil on error
   Write('<dd>%d seconds + %d nanoseconds\r\n' % {secs, nanos})
else
   err = nanos -- unix.Errno is always second result
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_DONTROUTE)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_DONTROUTE)\r\n')
if enabled then -- is nil if error
   Write('<dd>%d\r\n' % {enabled}) -- should be 0 or 1
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDBUF)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDBUF)\r\n')
if bytes then -- is nil if error
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVBUF)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVBUF)\r\n')
if bytes then -- is nil if error
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_FASTOPEN)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_FASTOPEN)\r\n')
if bytes then -- is nil if error
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_BROADCAST)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_BROADCAST)\r\n')
if enabled then -- is nil if error
   Write('<dd>%d\r\n' % {enabled}) -- should be 1 or 0
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_CORK)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_CORK)\r\n')
if enabled then -- is nil if error
   Write('<dd>%d\r\n' % {enabled}) -- should be 1 or 0
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_QUICKACK)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_QUICKACK)\r\n')
if enabled then
   Write('<dd>%d\r\n' % {enabled})
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_DEFER_ACCEPT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_DEFER_ACCEPT)\r\n')
if enabled then
   Write('<dd>%s\r\n' % {enabled})
else
   Write('<dd>%s\r\n' % {err})
end

enabled, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_FASTOPEN_CONNECT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_FASTOPEN_CONNECT)\r\n')
if err then
   Write('<dd>%s\r\n' % {err})
else
   Write('<dd>%s\r\n' % {enabled})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDLOWAT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDLOWAT)\r\n')
if bytes then
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVLOWAT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVLOWAT)\r\n')
if bytes then
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPCNT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPCNT)\r\n')
if bytes then
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_MAXSEG)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_MAXSEG)\r\n')
if bytes then
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_SYNCNT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_SYNCNT)\r\n')
if bytes then
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_NOTSENT_LOWAT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_NOTSENT_LOWAT)\r\n')
if bytes then
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_WINDOW_CLAMP)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_WINDOW_CLAMP)\r\n')
if bytes then
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPIDLE)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPIDLE)\r\n')
if bytes then
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

bytes, err = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPINTVL)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPINTVL)\r\n')
if bytes then
   Write('<dd>%d\r\n' % {bytes})
else
   Write('<dd>%s\r\n' % {err})
end

Write('<dt>unix.environ()\r\n')
Write('<dd>\r\n')
Write('<ul>\r\n')
env = unix.environ()
for i = 1,#env do
   Write('<li>%s\r\n' % {EscapeHtml(env[i])})
end
Write('</ul>\r\n')
