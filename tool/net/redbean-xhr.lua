-- redbean xhr handler demo
SetHeader('Vary', 'X-Custom-Header')
SetHeader('X-Custom-Header', 'hello ' .. GetHeader('x-custom-header'))
