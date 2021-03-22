send('HTTP/1.1 200 OK\r\n'..
     'Date: ' .. date() .. '\r\n'..
     'Server: redbean/0.1\r\n'..
     'Content-Type: text/plain\r\n'..
     'Content-Length: 7\r\n'..
     '\r\n'..
     'hello\r\n')
