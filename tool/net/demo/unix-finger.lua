-- UNIX Finger Example

local function WriteForm(host, user)
   Write([[<!doctype html>
     <title>redbean unix finger demo</title>
     <style>
       body { padding: 1em; }
       h1 a { color: inherit; text-decoration: none; }
       h1 img { border: none; vertical-align: middle; }
       input { margin: .5em; padding: .25em; }
       pre { margin-left: 2em; }
       p { word-break: break-word; max-width: 650px; }
       dt { font-weight: bold; }
       dd { margin-top: 1em; margin-bottom: 1em; }
       .hdr { text-indent: -1em; padding-left: 1em; }
     </style>
     <h1>
       <a href="/"><img src="/redbean.png"></a>
       <a href="unix-finger.lua">redbean unix finger demo</a>
     </h1>
     <p>
       Your redbean is able to function as an finger client. Lua server
       pages can use the <code>unix</code> module to implement protocols
       that your redbean wasn't originally intended to support. All it
       takes is few lines of code!
     </p>
     <form action="unix-finger.lua" method="post">
       <input type="text" id="host" name="host" size="40"
              value="%s" placeholder="host" autofocus>
       <label for="host">host</label>
       <br>
       <input type="text" id="user" name="user" size="40"
              value="%s" placeholder="user">
       <label for="user">user</label>
       <br>
       <input type="submit" value="finger">
     </form>
   ]] % {EscapeHtml(host), EscapeHtml(user)})
end

local function main()
   if IsPublicIp(GetClientAddr()) then
      ServeError(403)
   elseif GetMethod() == 'GET' or GetMethod() == 'HEAD' then
      WriteForm('graph.no', 'new_york')
   elseif GetMethod() == 'POST' then
      ip = assert(ResolveIp(GetParam('host')))
      fd = assert(unix.socket())
      assert(unix.connect(fd, ip, 79))
      assert(unix.write(fd, GetParam('user') .. '\r\n'))
      response = ''
      while true do
         data = assert(unix.read(fd))
         if data == '' then
            break
         end
         response = response .. data
      end
      assert(unix.close(fd))
      WriteForm(GetParam('host'), GetParam('user'))
      Write('<pre>\r\n')
      Write(EscapeHtml(VisualizeControlCodes(response)))
      Write('</pre>\r\n')
   else
      ServeError(405)
      SetHeader('Allow', 'GET, HEAD, POST')
   end
end

main()
