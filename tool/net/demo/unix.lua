local unix = require "unix"

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
   rc, errno = unix.fork()

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
      Write(string.format('<span style="color:red">&nbsp;%s</span>\r\n', unix.strerrno(errno)))
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

   if rc ~= 0 then
      unix.close(fd)
      return
   end

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

   unix.write(fd, '<h2>listing of current directory</h2>\r\n')
   dir, err = unix.opendir('.')
   if dir then
      unix.write(fd, '<ul>\r\n')
      while true do
         name, kind, ino, off = dir:read()
         if not name then
            break
         end
         unix.write(fd, '<li>')
         unix.write(fd, EscapeHtml(VisualizeControlCodes(name)))
         if kind == unix.DT_DIR then
            unix.write(fd, '/')
         else
            st, err = unix.stat(name)
            if st then
               unix.write(fd, string.format(' (%d bytes)', st:size()))
            end
         end
         unix.write(fd, '\r\n')
      end
      unix.write(fd, '</ul>\r\n')
   else
      unix.write(fd, '<p>\r\n')
      unix.write(fd, string.format('failed: %s\r\n', EscapeHtml(VisualizeControlCodes(unix:strerror(err)))))
      unix.write(fd, '</p>\r\n')
   end

   -- terminate
   unix.close(fd)
   unix.exit(0)

end

main()
