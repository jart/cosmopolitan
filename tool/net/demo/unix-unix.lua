-- UNIX Domain Sockets Example

-- So we can detect that child died.
died = false
function OnSigchld(sig)
   died = true
   unix.wait() -- Prevent it from becoming a zombie
end
assert(unix.sigaction(unix.SIGCHLD, OnSigchld))

-- So keyboard interurpts only go to subprocess.
oldint = assert(unix.sigaction(unix.SIGINT, unix.SIG_IGN))
oldquit = assert(unix.sigaction(unix.SIGQUIT, unix.SIG_IGN))

-- UNIX domain sockets need a file
tmpdir = os.getenv('TMPDIR') or '/tmp'
unixpath = '%s/redbean-unix.sock.%d' % {tmpdir, unix.getpid()}

-- Create child process which is the server.
child = assert(unix.fork())
if child == 0 then
   server = assert(unix.socket(unix.AF_UNIX, unix.SOCK_STREAM))
   assert(unix.setsockopt(server, unix.SOL_SOCKET, unix.SO_RCVTIMEO, 2))
   assert(unix.bind(server, unixpath))
   assert(unix.listen(server))
   client = assert(unix.accept(server))
   data = assert(unix.read(client))
   assert(data == 'ping!')
   assert(assert(unix.write(client, 'pong!')) == 5)
   assert(unix.close(client))
   unix.exit(0)
end

-- Wait for the child to create the the socket file.
function FileExists(path)
   st, err = unix.stat(path)
   return not err
end
expobackoff = 1
while not died do
   if FileExists(unixpath) then
      break
   else
      expobackoff = expobackoff << 1
      unix.nanosleep(expobackoff // 1000000000,
                     expobackoff % 1000000000)
   end
end

-- Now connect to the socket.
if not died then
   client = assert(unix.socket(unix.AF_UNIX, unix.SOCK_STREAM))
   assert(unix.connect(client, unixpath))
   assert(assert(unix.write(client, 'ping!')) == 5)
   data = assert(unix.read(client))
   assert(data == 'pong!')
   itworked = true
else
   itworked = false
end

-- Wait for client to terminate. We don't check error here because if
-- the child already died and the signal handler reaped it, then this
-- returns a ECHILD error which is fine.
unix.wait()
assert(itworked)

-- Now clean up the socket file.
unix.unlink(unixpath)

SetStatus(200)
SetHeader('Connection', 'close') -- be lazy and let _Exit() clean up signal handlers
SetHeader('Content-Type', 'text/html; charset=utf-8')
Write('<!doctype html>\r\n')
Write('<title>redbean unix domain sockets example</title>\r\n')
Write('<h1>\r\n')
Write('<img style="vertical-align:middle" src="data:image/png;base64,\r\n')
Write(EncodeBase64(LoadAsset('/redbean.png')))
Write('">\r\n')
Write('redbean unix domain sockets example\r\n')
Write('</h1>\r\n')
Write([[
  <p>
    <strong>It worked!</strong> We successfully sent a ping
    pong via UNIX local sockets. Please check out the source
    code to this example inside your redbean at unix-unix.lua.
  </p>
]])
Write('</h1>\r\n')
