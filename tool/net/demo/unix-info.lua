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
sid, errno = unix.getsid(0)
if sid then
   Write('<dd>%d\r\n' % {sid})
else
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
end

Write('<dt>unix.gethostname()\r\n')
Write('<dd>%s\r\n' % {EscapeHtml(unix.gethostname())})
Write('<dt>unix.getcwd()\r\n')
Write('<dd>%s\r\n' % {EscapeHtml(unix.getcwd())})

function PrintResourceLimit(name, id)
   soft, errno, hard = unix.getrlimit(id)
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
      Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
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
ifs, errno = unix.siocgifconf()
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
   Write('%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_DEBUG)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_DEBUG)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_ACCEPTCONN)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_ACCEPTCONN)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_REUSEADDR)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_REUSEADDR)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_REUSEPORT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_REUSEPORT)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_KEEPALIVE)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_KEEPALIVE)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_NODELAY)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_NODELAY)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

secs, errno, micros = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVTIMEO)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVTIMEO)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d sec %d µs\r\n' % {secs, micros})
end

secs, errno, micros = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDTIMEO)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDTIMEO)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d sec %d µs\r\n' % {secs, micros})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_DONTROUTE)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_DONTROUTE)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDBUF)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDBUF)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVBUF)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVBUF)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_FASTOPEN)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_FASTOPEN)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_BROADCAST)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_BROADCAST)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_CORK)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_CORK)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_QUICKACK)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_QUICKACK)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_DEFER_ACCEPT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_DEFER_ACCEPT)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

enabled, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_FASTOPEN_CONNECT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_FASTOPEN_CONNECT)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%s\r\n' % {enabled})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDLOWAT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_SNDLOWAT)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVLOWAT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_SOCKET, unix.SO_RCVLOWAT)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPCNT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPCNT)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_MAXSEG)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_MAXSEG)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_SYNCNT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_SYNCNT)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_NOTSENT_LOWAT)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_NOTSENT_LOWAT)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_WINDOW_CLAMP)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_WINDOW_CLAMP)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPIDLE)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPIDLE)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

bytes, errno = unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPINTVL)
Write('<dt>unix.getsockopt(GetClientFd(), unix.SOL_TCP, unix.TCP_KEEPINTVL)\r\n')
if errno then
   Write('<dd>%s\r\n' % {EscapeHtml(unix.strerrno(errno))})
else
   Write('<dd>%d\r\n' % {bytes})
end

Write('<dt>unix.environ()\r\n')
Write('<dd>\r\n')
Write('<ul>\r\n')
env = unix.environ()
for i = 1,#env do
   Write('<li>%s\r\n' % {EscapeHtml(env[i])})
end
Write('</ul>\r\n')
