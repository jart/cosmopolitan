-- Integration test for cosmo.http.serve()
-- This test forks a child process to run the server, then sends HTTP requests

local cosmo = require("cosmo")
local http = require("cosmo.http")
local unix = require("cosmo.unix")

local INADDR_LOOPBACK = cosmo.ParseIp("127.0.0.1")
local INADDR_ANY = 0

-- Helper to send HTTP request and get response
local function http_request(port, method, path)
  local fd = unix.socket(unix.AF_INET, unix.SOCK_STREAM, 0)
  if not fd then
    return nil, "failed to create socket"
  end

  local ok, err = unix.connect(fd, INADDR_LOOPBACK, port)
  if not ok then
    unix.close(fd)
    return nil, "failed to connect: " .. tostring(err)
  end

  local req = method .. " " .. path .. " HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
  unix.send(fd, req)

  local response = ""
  while true do
    local data = unix.recv(fd, 4096)
    if not data or #data == 0 then
      break
    end
    response = response .. data
  end

  unix.close(fd)
  return response
end

-- Parent binds to port 0 to get a random available port
local server_fd = unix.socket(unix.AF_INET, unix.SOCK_STREAM, 0)
unix.setsockopt(server_fd, unix.SOL_SOCKET, unix.SO_REUSEADDR, 1)
unix.bind(server_fd, INADDR_ANY, 0)
local _, PORT = unix.getsockname(server_fd)
unix.close(server_fd)

-- Fork child to run server
local pid = unix.fork()
if not pid then
  error("fork failed")
end

if pid == 0 then
  -- Child: run server on the same port (SO_REUSEADDR allows this)
  http.serve({
    addr = ":" .. PORT,
    timeout = 5
  }, function(req)
    if req.uri == "/" then
      return {
        status = 200,
        headers = {["Content-Type"] = "text/plain"},
        body = "Hello from test server"
      }
    elseif req.uri == "/json" then
      return {
        status = 200,
        headers = {["Content-Type"] = "application/json"},
        body = '{"test": true}'
      }
    else
      return {
        status = 404,
        body = "Not Found"
      }
    end
  end)
  unix.exit(0)
else
  -- Parent: wait for server to start, then send requests
  unix.nanosleep(0, 100000000)  -- 100ms

  local function check(desc, cond)
    if not cond then
      unix.kill(pid, unix.SIGTERM)
      unix.wait(pid)
      error("FAILED: " .. desc)
    end
  end

  -- Test 1: GET /
  local resp1, err1 = http_request(PORT, "GET", "/")
  check("GET / should succeed", resp1)
  check("GET / should return 200", resp1:find("HTTP/1.1 200"))
  check("GET / should have content-type", resp1:lower():find("content%-type: text/plain"))
  check("GET / should have body", resp1:find("Hello from test server"))

  -- Test 2: GET /json
  local resp2, err2 = http_request(PORT, "GET", "/json")
  check("GET /json should succeed", resp2)
  check("GET /json should return 200", resp2:find("HTTP/1.1 200"))
  check("GET /json should have json content-type", resp2:lower():find("content%-type: application/json"))
  check("GET /json should have json body", resp2:find('"test": true'))

  -- Test 3: GET /notfound (404)
  local resp3, err3 = http_request(PORT, "GET", "/notfound")
  check("GET /notfound should succeed", resp3)
  check("GET /notfound should return 404", resp3:find("HTTP/1.1 404"))

  -- Test 4: POST request
  local resp4, err4 = http_request(PORT, "POST", "/")
  check("POST / should succeed", resp4)
  check("POST / should return 200", resp4:find("HTTP/1.1 200"))

  -- Cleanup: kill server
  unix.kill(pid, unix.SIGTERM)
  unix.wait(pid)

  print("all http server tests passed")
end
