-- /.init.lua is loaded at startup in redbean's main process

HidePath('/usr/share/zoneinfo/')

function OnHttpRequest()
   if HasParam('magic') then
      Write('<p>\r\n')
      Write('OnHttpRequest() has intercepted your request<br>\r\n')
      Write('because you specified the magic parameter\r\n')
      Write('<pre>\r\n')
      Write(EscapeHtml(LoadAsset('/.init.lua')))
      Write('</pre>\r\n')
   else
      Route()
   end
   SetHeader('Server', 'redbean!')
end
