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
   Write('<!doctype html>\n')
   Write('<title>redbean</title>\n')
   Write('<h1>redbean lua server page demo</h1>\n')

   -- Prevent caching.
   -- We need this because we're doing things like putting the client's
   -- IP address in the response so we naturally don't want that cached
   SetHeader('Expires', FormatHttpDateTime(GetDate()))
   SetHeader('Cache-Control', 'no-cache, must-revalidate, max-age=0')

   -- GetParams() returns an ordered list of Request-URI query params.
   Write('<h3>request uri parameters</h3>\n')
   params = GetParams()
   if #params > 0 then
      Write('<dl>\n')
      for i = 1,#params do
         Write('<dt>')
         Write(EscapeHtml(params[i][1]))
         Write('\n')
         if params[i][2] then
            Write('<dd>')
            Write(EscapeHtml(params[i][2]))
            Write('\n')
         end
      end
      Write('</dl>\n')
   else
      Write('<p>\n')
      Write('<em>none</em><br>\n')
      Write('ProTip: Try <a href="')
      Write(EscapeHtml(EscapePath(GetPath()) .. '?x=hi%20there&y&z&z=' .. EscapeParam('&')))
      Write('">clicking here</a>!\n')
   end

   -- Access redbean command line arguments.
   -- These are the ones that come *after* the redbean server arguments.
   Write('<h3>command line arguments</h3>\n')
   if #argv > 0 then
      Write('<ul>\n')
      for i = 1,#argv do
         Write('<li>')
         Write(EscapeHtml(argv[i]))
         Write('\n')
      end
      Write('</ul>\n')
   else
      Write('<p><em>none</em>\n')
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

   Write('<h3>extra information</h3>\n')
   Write('<dt>GetClientAddr()\n')
   Write('<dd>')
   Write(GetClientAddr())
   Write('\n')
   Write('<dt>GetServerAddr()\n')
   Write('<dd>')
   Write(GetServerAddr())
   Write('\n')
   Write('</dl>\n')
end

main()
