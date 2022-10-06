-- redbean lua server page demo

local function main()
   -- This is the best way to print data to the console or log file.
   Log(kLogWarn, "hello from \e[1mlua\e[0m!")

   -- This check is pedantic but might be good to have.
   if GetMethod() ~= 'GET' and GetMethod() ~= 'HEAD' then
      ServeError(405)
      SetHeader('Allow', 'GET, HEAD')
      return
   end

   -- These two lines are optional.
   -- The default behavior is to do this if you don't.
   SetStatus(200) -- Shorthand for SetStatus(200, "OK")
   SetHeader('Content-Type', 'text/html; charset=utf-8')

   -- Response data is buffered until the script finishes running.
   -- Compression is applied automatically, based on your headers.
   Write('<!doctype html>\r\n')
   Write('<title>redbean</title>\r\n')
   Write('<h1>\r\n')
   Write('<img style="vertical-align:middle" src="data:image/png;base64,\r\n')
   Write(EncodeBase64(LoadAsset('/redbean.png')))
   Write('">\r\n')
   Write('redbean lua server page demo\r\n')
   Write('</h1>\r\n')

   -- Prevent caching.
   -- We need this because we're doing things like putting the client's
   -- IP address in the response so we naturally don't want that cached
   SetHeader('Expires', FormatHttpDateTime(GetDate()))
   SetHeader('Cache-Control', 'no-cache, must-revalidate, max-age=0')

   -- GetUrl() is the resolved Request-URI (TODO: Maybe change API to return a URL object?)
   Write('<p>Thank you for visiting <code>')
   Write(GetUrl())  -- redbean encoded this value so it doesn't need html entity escaping
   Write('</code>\r\n')

   -- GetParam(NAME) is the fastest easiest way to get URL and FORM params
   -- If you want the RequestURL query params specifically in full do this
   Write('<h3>request url parameters</h3>\r\n')
   params = ParseUrl(GetUrl()).params  -- like GetParams() but w/o form body
   if params and #params>0 then
      Write('<dl>\r\n')
      for i = 1,#params do
         Write('<dt>')
         Write(EscapeHtml(VisualizeControlCodes(params[i][1])))
         Write('\r\n')
         if params[i][2] then
            Write('<dd>')
            Write(EscapeHtml(VisualizeControlCodes(params[i][2])))
            Write('\r\n')
         end
      end
      Write('</dl>\r\n')
      Write("<p>Whatever you do, don't click on ")
      Write('<a href="')
      Write(EscapeHtml(EscapePath(GetPath()) .. '?magic'))
      Write('">')
      Write(EscapeHtml(VisualizeControlCodes(GetPath())))
      Write('?magic</a>\r\n')
   else
      Write('<p>\r\n')
      Write('<em>none</em><br>\r\n')
      Write('ProTip: Try <a href="')
      Write(EscapeHtml(EscapePath(GetPath()) .. '?x=hi%20there%00%C0%80&y&z&z=' .. EscapeParam('&')))
      Write('">clicking here</a>!\r\n')
   end

   -- redbean command line arguments
   -- these come *after* the c getopt server arguments
   Write('<h3>command line arguments</h3>\r\n')
   if #arg > 0 then
      Write('<ul>\r\n')
      for i = 1,#arg do
         Write('<li>')
         Write(EscapeHtml(VisualizeControlCodes(arg[i])))
         Write('\r\n')
      end
      Write('</ul>\r\n')
   else
      Write('<p><em>none</em>\r\n')
   end

   Write([[
     <h3>post request html form demo</h3>
     <form action="redbean-form.lua" method="post">
     <input type="text" id="firstname" name="firstname">
     <label for="firstname">first name</label>
     <br>
     <input type="text" id="lastname" name="lastname">
     <label for="lastname">last name</label>
     <br>
     <input type="submit" value="Submit">
     </form>
   ]])

   Write([[
     <h3>xmlhttprequest request demo</h3>
     <input id="x" value="lâtìn1">
     <label for="x">X-Custom-Header</label><br>
     <button id="send">send</button><br>
     <div id="result"></div>
     <script>
       function OnSend() {
         var r = new XMLHttpRequest();
         r.onload = function() {
           document.getElementById("result").innerText = this.getResponseHeader('X-Custom-Header');
         };
         r.open('POST', 'redbean-xhr.lua');
         r.setRequestHeader('X-Custom-Header', document.getElementById('x').value);
         r.send();
       }
       document.getElementById('send').onclick = OnSend;
     </script>
   ]])

   -- fast redbean apis for accessing already parsed request data
   Write('<h3>extra information</h3>\r\n')
   Write('<dl>\r\n')
   Write('<dt>GetMethod()\r\n')
   Write('<dd>')
   Write(EscapeHtml(GetMethod()))  -- & and ' are legal in http methods
   Write('\r\n')
   if GetUser() then
      Write('<dt>GetUser()\r\n')
      Write('<dd>')
      Write(EscapeHtml(VisualizeControlCodes(GetUser())))
      Write('\r\n')
   end
   if GetScheme() then
      Write('<dt>GetScheme()\r\n')
      Write('<dd>')
      Write(GetScheme())
      Write('\r\n')
   end
   if GetPass() then
      Write('<dt>GetPass()\r\n')
      Write('<dd>')
      Write(EscapeHtml(VisualizeControlCodes(GetPass())))
      Write('\r\n')
   end
   Write('<dt>GetHost() <small>(from HTTP Request-URI or Host header or X-Forwarded-Host header or Berkeley Sockets)</small>\r\n')
   Write('<dd>')
   Write(EscapeHtml(VisualizeControlCodes(GetHost())))
   Write('\r\n')
   Write('<dt>GetPort() <small>(from HTTP Request-URI or Host header or X-Forwarded-Host header or Berkeley Sockets)</small>\r\n')
   Write('<dd>')
   Write(tostring(GetPort()))
   Write('\r\n')
   Write('<dt>GetPath() <small>(from HTTP Request-URI)</small>\r\n')
   Write('<dd>')
   Write(EscapeHtml(VisualizeControlCodes(GetPath())))
   Write('\r\n')
   Write('<dt>GetEffectivePath() <small>(actual path used internally to load the lua asset: routed depending on host, request path, and rewrites)</small>\r\n')
   Write('<dd>')
   Write(EscapeHtml(VisualizeControlCodes(GetEffectivePath())))
   Write('\r\n')
   if GetFragment() then
      Write('<dt>GetFragment()\r\n')
      Write('<dd>')
      Write(EscapeHtml(VisualizeControlCodes(GetFragment())))
      Write('\r\n')
   end
   Write('<dt>GetRemoteAddr() <small>(from Berkeley Sockets or X-Forwarded-For header)</small>\r\n')
   Write('<dd>')
   ip, port = GetRemoteAddr()
   Write('%s, %d' % {FormatIp(ip), port})
   if CategorizeIp(ip) then
      Write('<br>\r\n')
      Write(CategorizeIp(ip))
   end
   Write('\r\n')
   Write('<dt>GetClientAddr()\r\n')
   Write('<dd>')
   ip, port = GetClientAddr()
   Write('%s, %d' % {FormatIp(ip), port})
   if CategorizeIp(ip) then
      Write('<br>\r\n')
      Write(CategorizeIp(ip))
   end
   Write('\r\n')
   Write('<dt>GetServerIp()\r\n')
   Write('<dd>')
   ip, port = GetServerAddr()
   Write('%s, %d' % {FormatIp(ip), port})
   if CategorizeIp(ip) then
      Write('<br>\r\n')
      Write(CategorizeIp(ip))
   end
   Write('\r\n')
   Write('</dl>\r\n')

   -- redbean apis for generalized parsing and encoding
   referer = GetHeader('Referer')
   if referer then
      url = ParseUrl(referer)
      if url.scheme then
         url.scheme = string.upper(url.scheme)
      end
      Write('<h3>referer url</h3>\r\n')
      Write('<p>\r\n')
      Write(EscapeHtml(EncodeUrl(url)))
      Write('<dl>\r\n')
      if url.scheme then
         Write('<dt>scheme\r\n')
         Write('<dd>\r\n')
         Write(url.scheme)
      end
      if url.user then
         Write('<dt>user\r\n')
         Write('<dd>\r\n')
         Write(EscapeHtml(VisualizeControlCodes(url.user)))
      end
      if url.pass then
         Write('<dt>pass\r\n')
         Write('<dd>\r\n')
         Write(EscapeHtml(VisualizeControlCodes(url.pass)))
      end
      if url.host then
         Write('<dt>host\r\n')
         Write('<dd>\r\n')
         Write(EscapeHtml(VisualizeControlCodes(url.host)))
      end
      if url.port then
         Write('<dt>port\r\n')
         Write('<dd>\r\n')
         Write(EscapeHtml(VisualizeControlCodes(url.port)))
      end
      if url.path then
         Write('<dt>path\r\n')
         Write('<dd>\r\n')
         Write(EscapeHtml(VisualizeControlCodes(url.path)))
      end
      if url.params then
         Write('<dt>params\r\n')
         Write('<dd>\r\n')
         Write('<dl>\r\n')
         for i = 1,#url.params do
            Write('<dt>')
            Write(EscapeHtml(VisualizeControlCodes(url.params[i][1])))
            Write('\r\n')
            if url.params[i][2] then
               Write('<dd>')
               Write(EscapeHtml(VisualizeControlCodes(url.params[i][2])))
               Write('\r\n')
            end
         end
         Write('</dl>\r\n')
      end
      if url.fragment then
         Write('<dt>fragment\r\n')
         Write('<dd>\r\n')
         Write(EscapeHtml(VisualizeControlCodes(url.fragment)))
      end
      Write('</dl>\r\n')
   end

   Write('<h3>posix extended regular expressions</h3>\r\n')
   s = 'my ' .. FormatIp(GetRemoteAddr()) .. ' ip'
   -- traditional regular expressions
   m,a,b,c,d = re.search([[\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)]], s, re.BASIC)
   -- easy api (~10x slower because compile is O(2ⁿ) and search is O(n))
   m,a,b,c,d = re.search([[([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})]], s)
   -- proper api (especially if you put the re.compile() line in /.init.lua)
   pat = re.compile([[([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})]])
   m,a,b,c,d = pat:search(s)  -- m and rest are nil if match not found
   Write('<pre>\r\n')
   Write([[pat = re.compile('([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})')]])
   Write('\r\nm,a,b,c,d = pat:search(%q)\r\n' % {s})
   Write('</pre>\r\n')
   Write('<dl>\r\n')
   Write('<dt>m\r\n')
   Write('<dd>')
   Write("%q" % {m})
   Write('\r\n')
   Write('<dt>a\r\n')
   Write('<dd>')
   Write("%q" % {a})
   Write('\r\n')
   Write('<dt>b\r\n')
   Write('<dd>')
   Write("%q" % {b})
   Write('\r\n')
   Write('<dt>c\r\n')
   Write('<dd>')
   Write("%q" % {c})
   Write('\r\n')
   Write('<dt>d\r\n')
   Write('<dd>')
   Write("%q" % {d})
   Write('\r\n')
   Write('</dl>\r\n')

   Write('<h3>source code to this page</h3>\r\n')
   Write('<pre>\r\n')
   Write(EscapeHtml(LoadAsset(GetEffectivePath())))
   Write('</pre>\r\n')

   -- redbean zip assets
   Write('<h3>zip assets</h3>\r\n')
   paths = GetZipPaths()
   if #paths > 0 then
      Write('<ul>\r\n')
      for i = 1,#paths do
         Write('<li>\r\n')
         Write('<a href="')
         Write(EscapeHtml(EscapePath(paths[i])))
         Write('">')
         Write(EscapeHtml(VisualizeControlCodes(paths[i])))
         Write('</a>')
         if IsHiddenPath(paths[i]) then
            Write(' <small>[HIDDEN]</small>')
         end
         if not IsAcceptablePath(paths[i]) then
            Write(' <small>[BLOCKED]</small>')
         end
         if not IsAssetCompressed(paths[i]) then
            Write(' <small>[UNCOMPRESSED]</small>')
         end
         if (GetAssetMode(paths[i]) & 0xF000) == 0x4000 then
            Write(' <small>[DIRECTORY]</small>')
         end
         Write('<br>\r\n')
         Write('Modified: ')
         Write(FormatHttpDateTime(GetAssetLastModifiedTime(paths[i])))
         Write('<br>\r\n')
         Write('Mode: ')
         Write("0%o" % {GetAssetMode(paths[i])})
         Write('<br>\r\n')
         Write('Size: ')
         Write(tostring(GetAssetSize(paths[i])))
         Write('<br>\r\n')
         if GetAssetComment(paths[i]) then
            Write('Comment: ')
            Write(EscapeHtml(VisualizeControlCodes(GetAssetComment(paths[i]))))
            Write('<br>\r\n')
         end
         Write('\r\n')
      end
      Write('</ul>\r\n')
   else
      Write('<p><em>none</em>\r\n')
   end

end

main()
