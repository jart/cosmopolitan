-- Fetch() API Demo

local function WriteForm(url)
   Write('<!doctype html>\r\n')
   Write(string.format([[
     <title>redbean fetch demo</title>
     <style>
       body {
         padding: 1em;
       }
       h1 a {
         color: inherit;
         text-decoration: none;
       }
       h1 img {
         border: none;
         vertical-align: middle;
       }
       input {
         margin: 1em;
         padding: .5em;
       }
       p {
         word-break: break-word;
       }
       p span {
         display: block;
         text-indent: -1em;
         padding-left: 1em;
       }
     </style>
     <h1>
       <a href="/"><img src="/redbean.png"></a>
       <a href="fetch.lua">redbean fetch demo</a>
     </h1>
     <form action="fetch.lua" method="post">
       <input type="text" id="url" name="url" size="70"
              value="%s" placeholder="uri" autofocus>
       <input type="submit" value="fetch">
     </form>
   ]], EscapeHtml(url)))
end

local function main()
   if IsPublicIp(GetClientAddr()) then
      ServeError(403)
   elseif GetMethod() == 'GET' or GetMethod() == 'HEAD' then
      WriteForm("https://www.cloudflare.com/robots.txt")
   elseif GetMethod() == 'POST' then
      status, headers, payload = Fetch(GetParam('url'))
      WriteForm(GetParam('url'))
      Write('<dl>\r\n')
      Write('<dt>Status\r\n')
      Write(string.format('<dd><p>%d %s\r\n', status, GetHttpReason(status)))
      Write('<dt>Headers\r\n')
      Write('<dd><p>\r\n')
      for k,v in pairs(headers) do
         Write('<span><strong>')
         Write(EscapeHtml(k))
         Write('</strong>: ')
         Write(EscapeHtml(v))
         Write('</span>\r\n')
      end
      Write('<dt>Payload\r\n')
      Write('<dd><pre>')
      Write(EscapeHtml(VisualizeControlCodes(payload)))
      Write('</pre>\r\n')
      Write('</dl>\r\n')
   else
      ServeError(405)
      SetHeader('Allow', 'GET, HEAD, POST')
   end
end

main()
