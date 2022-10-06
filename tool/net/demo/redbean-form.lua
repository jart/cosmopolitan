-- redbean post request handler demo

local function main()
   if GetMethod() ~= 'POST' then
      ServeError(405)
      SetHeader('Allow', 'POST')
      return
   end
   SetStatus(200)
   SetHeader('Content-Type', 'text/html; charset=utf-8')
   Write('<!doctype html>\r\n')
   Write('<title>redbean</title>\r\n')
   Write('<h3>POST Request HTML Form Handler Demo</h3>\r\n')

   Write('<p>')
   firstname = GetParam('firstname')
   lastname = GetParam('lastname')
   if firstname and lastname then
      Write('Hello ')
      Write(EscapeHtml(VisualizeControlCodes(firstname)))
      Write(' ')
      Write(EscapeHtml(VisualizeControlCodes(lastname)))
      Write('!<br>')
      Write('Thank you for using redbean.')
   end

   Write('<dl>\r\n')

   Write('<dt>Params\r\n')
   Write('<dd>\r\n')
   Write('<dl>\r\n')
   params = GetParams()
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

   Write('<dt>Headers\r\n')
   Write('<dd>\r\n')
   Write('<dl>\r\n')
   for k,v in pairs(GetHeaders()) do
      Write('<dt>')
      Write(EscapeHtml(k))
      Write('\r\n')
      Write('<dd>')
      Write(EscapeHtml(v))
      Write('\r\n')
   end
   Write('</dl>\r\n')

   Write('<dt>Payload\r\n')
   Write('<dd><p>')
   Write(EscapeHtml(VisualizeControlCodes(GetBody())))
   Write('\r\n')

   Write('</dl>\r\n')

   Write('<p>')
   Write('<a href="redbean.lua">Click here</a> ')
   Write('to return to the previous page.\r\n')
end

main()
