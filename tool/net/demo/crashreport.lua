globalvar1 = 31337

globalvar2 = {
   hello = 'world',
}

function dosomething(param1, x)
   local res
   local y
   y = 0
   SetStatus(200)
   SetHeader('Content-Type', 'text/plain; charset=utf-8')
   Write('preprae to crash... now\r\n')
   res = x / y
   Write('42 / 0 is %d\r\n' % {res})
end

function start(param1)
   local x = 42
   dosomething(s, x)
end

function main()
   local s = 'hello'
   start(s)
end

main()
