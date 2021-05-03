-- /.init.lua is loaded at startup in redbean's main process
HidePath('/usr/share/zoneinfo/')

-- open a browser tab using explorer/open/xdg-open
-- LaunchBrowser('/tool/net/demo/index.html')

-- this intercepts all requests if it's defined
function OnHttpRequest()
   if HasParam('magic') then
      Write('<p>\r\n')
      Write('OnHttpRequest() has intercepted your request<br>\r\n')
      Write('because you specified the magic parameter\r\n')
      Write('<pre>\r\n')
      Write(EscapeHtml(LoadAsset('/.init.lua')))
      Write('</pre>\r\n')
   else
      Route() -- this asks redbean to do the default thing
   end
   SetHeader('Server', 'redbean!')
end
