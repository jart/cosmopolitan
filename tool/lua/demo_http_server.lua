#!/usr/bin/env lua
-- Demo HTTP server using cosmo.http
--
-- Run with: o//tool/lua/lua.dbg tool/lua/demo_http_server.lua
-- Then visit: http://localhost:8080/
--
-- Press Ctrl+C to stop

local http = require("cosmo.http")

print("Starting HTTP server on :8080...")
print("Press Ctrl+C to stop")

http.serve(":8080", function(req)
  print(string.format("%s %s from %s", req.method, req.uri, req.client_ip))

  if req.uri == "/" then
    return {
      status = 200,
      headers = {["Content-Type"] = "text/html"},
      body = [[
<!DOCTYPE html>
<html>
<head><title>Hello from Lua!</title></head>
<body>
<h1>Hello from cosmo.http!</h1>
<p>This server is running in Lua using the cosmo.http module.</p>
<ul>
  <li><a href="/hello">Say hello</a></li>
  <li><a href="/json">Get JSON</a></li>
</ul>
</body>
</html>
]]
    }
  elseif req.uri == "/hello" then
    return {
      status = 200,
      headers = {["Content-Type"] = "text/plain"},
      body = "Hello, World!\n"
    }
  elseif req.uri == "/json" then
    return {
      status = 200,
      headers = {["Content-Type"] = "application/json"},
      body = '{"message": "Hello from Lua!", "status": "ok"}\n'
    }
  else
    return {
      status = 404,
      headers = {["Content-Type"] = "text/plain"},
      body = "Not Found\n"
    }
  end
end)
