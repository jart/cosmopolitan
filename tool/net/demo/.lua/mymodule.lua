local mymodule = {}

function mymodule.hello()
   SetStatus(200)
   SetHeader('Content-Type', 'text/html; charset=US-ASCII')
   Write("<!doctype html>\r\n")
   Write("<b>Hello World!</b>\r\n")
end

return mymodule
