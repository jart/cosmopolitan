-- redbean xhr handler demo
hdr = GetHeader('x-custom-header')
if hdr then
   SetHeader('Vary', 'X-Custom-Header')
   SetHeader('X-Custom-Header', 'hello ' .. hdr)
else
   ServeError(400)
end
