-- webserver within a webserver demo
-- we must go deeper!

local unix = require 'unix'

function OnTerm(sig)
   -- prevent redbean core from writing a response
   unix.write(mainfd, 'redbean is shutting down\r\n')
   unix.close(mainfd)
end

function main()
   if IsClientUsingSsl() then
      ServeError(400)
      return
   end

   mainfd = GetClientFd()
   mainip = GetRemoteAddr()
   unix.sigaction(unix.SIGTERM, OnTerm)
   unix.write(mainfd, 'HTTP/1.0 200 OK\r\n' ..
              'Date: '.. FormatHttpDateTime(GetDate()) ..'\r\n' ..
              'Content-Type: text/html; charset=utf-8\r\n' ..
              'Connection: close\r\n' ..
              'Server: redbean unix\r\n' ..
              '\r\n' ..
              '<!doctype html>\r\n' ..
              '<title>redbean</title>\r\n' ..
              '<h3>webserver within a webserver demo</h3>\r\n')

   addrs = {}
   servers = {}
   pollfds = {}
   pollfds[mainfd] = unix.POLLIN

   ifs = assert(unix.siocgifconf())
   for i = 1,#ifs do
      if (IsLoopbackIp(mainip) and (IsPublicIp(ifs[i].ip) or
                                    IsPrivateIp(ifs[i].ip) or
                                    IsLoopbackIp(ifs[i].ip))) or
         (IsPrivateIp(mainip) and (IsPublicIp(ifs[i].ip) or
                                   IsPrivateIp(ifs[i].ip))) or
         (IsPublicIp(mainip) and IsPublicIp(ifs[i].ip))
      then
         server = unix.socket()
         unix.bind(server, ifs[i].ip)
         unix.listen(server)
         ip, port = assert(unix.getsockname(server))
         addr = '%s:%d' % {FormatIp(ip), port}
         url = 'http://%s' % {addr}
         Log(kLogInfo, 'listening on %s' % {addr})
         unix.write(mainfd, 'listening on <a target="_blank" href="%s">%s</a><br>\r\n' % {url, url})
         pollfds[server] = unix.POLLIN | unix.POLLHUP
         servers[server] = true
         addrs[server] = addr
      end
   end

   while true do
      evs, errno = unix.poll(pollfds)
      if not evs then
         break
      end
      for fd,revents in pairs(evs) do
         if fd == mainfd then
            data, errno = unix.read(mainfd)
            if not data then
               Log(kLogInfo, 'got %s from parent client' % {tostring(errno)})
               -- prevent redbean core from writing a response
               unix.exit(1)
            end
            if #data == 0 then
               Log(kLogInfo, 'got hangup from parent client')
               Log(kLogInfo, 'closing server')
               -- prevent redbean core from writing a response
               unix.exit(0)
            end
            -- echo it back for fun
            unix.write(mainfd, data)
         elseif servers[fd] then
            unix.write(mainfd, 'preparing to accept from %d<br>\r\n' % {fd})
            client, clientip, clientport = assert(unix.accept(fd))
            addr = '%s:%d' % {FormatIp(clientip), clientport}
            addrs[client] = addr
            unix.write(mainfd, 'got client %s<br>\r\n' % {addr})
            pollfds[client] = unix.POLLIN
            evs[server] = nil
         else
            unix.write(mainfd, 'preparing to read from %d<br>\r\n' % {fd})
            data = unix.read(fd)
            unix.write(mainfd, 'done reading from %d<br>\r\n' % {fd})
            if data and #data ~= 0 then
               unix.write(mainfd, 'got %d bytes from %s<br>\r\n' % {#data, addrs[fd]})
               unix.write(fd, 'HTTP/1.0 200 OK\r\n' ..
                          'Date: '.. FormatHttpDateTime(GetDate()) ..'\r\n' ..
                          'Content-Type: text/html; charset=utf-8\r\n' ..
                          'Connection: close\r\n' ..
                          'Server: redbean unix\r\n' ..
                          '\r\n' ..
                          '<!doctype html>\r\n' ..
                          '<title>redbean</title>\r\n' ..
                          '<h3>embedded webserver demo</h3>\r\n' ..
                          'hello! this is a web server embedded in a web server!\r\n')
            end
            unix.close(fd)
            pollfds[fd] = nil
         end
      end
   end

end
main()
