local unix = require 'unix'

local function main()
   if GetMethod() ~= 'GET' and GetMethod() ~= 'HEAD' then
      ServeError(405)
      SetHeader('Allow', 'GET, HEAD')
      return
   end
   if IsClientUsingSsl() then
      SetStatus(400)
      SetHeader('Content-Type', 'text/html; charset=utf-8')
      Write('<!doctype html>\r\n')
      Write('<title>redbean unix module</title>\r\n')
      Write('<h1>\r\n')
      Write('<img style="vertical-align:middle" src="data:image/png;base64,\r\n')
      Write(EncodeBase64(LoadAsset('/redbean.png')))
      Write('">\r\n')
      Write('redbean unix demo\r\n')
      Write('<span style="color:red">&nbsp;error</span>\r\n')
      Write('</h1>\r\n')
      Write([[
        <p>
          The redbean unix module demo needs to intercept a raw unix
          socket. It's hard to do that if your file descriptor is
          encrypted with TLS. Please reload this page using http://
          rather than https://.
        </p>
      ]])
      Write('</h1>\r\n')
      return
   end

   -- steal client from redbean
   fd = GetClientFd()

   -- this function returns twice
   pid, errno = unix.fork()
   if errno then
      SetStatus(400)
      SetHeader('Content-Type', 'text/html; charset=utf-8')
      Write('<!doctype html>\r\n')
      Write('<title>redbean unix module</title>\r\n')
      Write('<h1>\r\n')
      Write('<img style="vertical-align:middle" src="data:image/png;base64,\r\n')
      Write(EncodeBase64(LoadAsset('/redbean.png')))
      Write('">\r\n')
      Write('redbean unix demo\r\n')
      Write('<span style="color:red">&nbsp;%s</span>\r\n' % {EscapeHtml(tostring(errno))})
      Write('</h1>\r\n')
      Write([[
        <p>
          Your redbean doesn't have the ability to fork. Most likely
          because you enabled sandboxing and fork() isn't in your bpf
          policy.
        </p>
      ]])
      Write('</h1>\r\n')
      return
   end

   -- the parent process gets the pid
   if pid ~= 0 then
      unix.close(fd)
      return
   end
   -- if pid is zero then we're the child
   -- turn into a daemon
   unix.umask(0)
   unix.setsid()
   if unix.fork() > 0 then unix.exit(0) end
   unix.close(0)
   unix.open('/dev/null', unix.O_RDONLY)
   unix.close(1)
   unix.open('/dev/null', unix.O_WRONLY)
   efd = unix.open('/tmp/redbean-unix.log', unix.O_WRONLY | unix.O_CREAT | unix.O_TRUNC, 0600)
   unix.dup(efd, 2)
   unix.close(efd)
   unix.sigaction(unix.SIGHUP, unix.SIG_DFL)
   unix.sigaction(unix.SIGINT, unix.SIG_DFL)
   unix.sigaction(unix.SIGQUIT, unix.SIG_DFL)
   unix.sigaction(unix.SIGTERM, unix.SIG_DFL)
   unix.sigaction(unix.SIGUSR1, unix.SIG_DFL)
   unix.sigaction(unix.SIGUSR2, unix.SIG_DFL)
   unix.sigaction(unix.SIGCHLD, unix.SIG_DFL)

   -- communicate with client
   unix.write(fd, 'HTTP/1.0 200 OK\r\n' ..
              'Connection: close\r\n' ..
              'Date: '.. FormatHttpDateTime(GetDate()) ..'\r\n' ..
              'Content-Type: text/html; charset=utf-8\r\n' ..
              'Server: redbean unix\r\n' ..
              '\r\n')
   unix.write(fd, '<!doctype html>\n')
   unix.write(fd, '<title>redbean unix module</title>')
   unix.write(fd, '<h1>\r\n')
   unix.write(fd, '<img style="vertical-align:middle" src="data:image/png;base64,\r\n')
   unix.write(fd, EncodeBase64(LoadAsset('/redbean.png')))
   unix.write(fd, '">\r\n')
   unix.write(fd, 'redbean unix demo\r\n')
   unix.write(fd, '<span style="color:green">&nbsp;success</span>\r\n')
   unix.write(fd, '</h1>\r\n')
   unix.write(fd, '<p>\r\n')
   unix.write(fd, 'your lua code just stole control of your http client\r\n')
   unix.write(fd, 'socket file descriptor from redbean server, and then\r\n')
   unix.write(fd, 'became an autonomous daemon reparented on your init!\r\n')
   unix.write(fd, '</p>\r\n')

   -- terminate
   unix.close(fd)
   unix.exit(0)

end

main()
