-- redbean post request handler demo

local function main()
   if GetMethod() ~= 'POST' then
      ServeError(405)
      SetHeader('Allow', 'POST')
      return
   end
   SetStatus(200)
   SetHeader('Content-Type', 'text/html; charset=utf-8')
   Write('<!doctype html>\n')
   Write('<title>redbean</title>\n')
   Write('<h3>POST Request HTML Form Handler Demo</h3>\n')

   Write('<p>')
   firstname = GetParam('firstname')
   lastname = GetParam('lastname')
   if firstname and lastname then
      Write('Hello ')
      Write(EscapeHtml(firstname))
      Write(' ')
      Write(EscapeHtml(lastname))
      Write('!<br>')
      Write('Thank you for using redbean.')
   end

   Write('<dl>\n')

   Write('<dt>Params\n')
   Write('<dd>\n')
   Write('<dl>\n')
   params = GetParams()
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

   Write('<dt>Headers\n')
   Write('<dd>\n')
   Write('<dl>\n')
   for k,v in pairs(GetHeaders()) do
      Write('<dt>')
      Write(EscapeHtml(k))
      Write('\n')
      Write('<dd>')
      Write(EscapeHtml(v))
      Write('\n')
   end
   Write('</dl>\n')

   Write('<dt>Payload\n')
   Write('<dd><p>')
   Write(EscapeHtml(GetPayload()))
   Write('\n')

   Write('</dl>\n')

   Write('<p>')
   Write('<a href="/tool/net/redbean.lua">Click here</a> ')
   Write('to return to the previous page.\n')
end

main()
