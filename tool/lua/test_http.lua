local http = require("cosmo.http")

-- test module exists
assert(type(http) == "table", "http module should be a table")
assert(type(http.parse) == "function", "http.parse should be a function")
assert(type(http.parse_response) == "function", "http.parse_response should be a function")
assert(type(http.format_response) == "function", "http.format_response should be a function")
assert(type(http.format_request) == "function", "http.format_request should be a function")
assert(type(http.reason) == "function", "http.reason should be a function")

-- test constants
assert(http.OK == 200, "http.OK should be 200")
assert(http.NOT_FOUND == 404, "http.NOT_FOUND should be 404")
assert(http.INTERNAL_SERVER_ERROR == 500, "http.INTERNAL_SERVER_ERROR should be 500")

-- test http.parse
local req = http.parse("GET /hello HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: test\r\n\r\n")
assert(req, "http.parse should return a table")
assert(req.method == "GET", "method should be GET, got: " .. tostring(req.method))
assert(req.uri == "/hello", "uri should be /hello, got: " .. tostring(req.uri))
assert(req.version == 11, "version should be 11, got: " .. tostring(req.version))
assert(req.headers, "headers should exist")
assert(req.headers.Host == "localhost:8080", "Host header should be localhost:8080")
assert(req.headers["User-Agent"] == "test", "User-Agent header should be test")

-- test http.parse with body
local req_body = http.parse("POST /api HTTP/1.1\r\nHost: localhost\r\nContent-Length: 13\r\n\r\nHello, World!")
assert(req_body, "http.parse should return a table for POST")
assert(req_body.method == "POST", "method should be POST")
assert(req_body.body == "Hello, World!", "body should be 'Hello, World!', got: " .. tostring(req_body.body))

-- test http.parse error
local bad, err = http.parse("not http")
assert(bad == nil, "invalid HTTP should return nil")
assert(err, "should return error message")

-- test http.parse incomplete
local incomplete, err2 = http.parse("GET /hello HTTP/1.1\r\n")
assert(incomplete == nil, "incomplete HTTP should return nil")
assert(err2, "should return error message")

-- test http.parse_response
local resp = http.parse_response("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello</h1>")
assert(resp, "http.parse_response should return a table")
assert(resp.status == 200, "status should be 200")
assert(resp.message == "OK", "message should be OK")
assert(resp.version == 11, "version should be 11")
assert(resp.headers["Content-Type"] == "text/html", "Content-Type should be text/html")
assert(resp.body == "<h1>Hello</h1>", "body should be '<h1>Hello</h1>'")

-- test http.format_response
local formatted = http.format_response({
  status = 200,
  headers = {["Content-Type"] = "text/plain"},
  body = "Hello"
})
assert(formatted:find("HTTP/1.1 200 OK"), "should contain status line")
assert(formatted:find("Content%-Type: text/plain"), "should contain Content-Type header")
assert(formatted:find("\r\n\r\nHello"), "should contain body after headers")

-- test http.format_response with default status
local default_resp = http.format_response({body = "test"})
assert(default_resp:find("HTTP/1.1 200 OK"), "should default to 200 OK")

-- test http.format_request
local req_formatted = http.format_request({
  method = "POST",
  uri = "/api/data",
  headers = {Host = "example.com"},
  body = '{"key":"value"}'
})
assert(req_formatted:find("POST /api/data HTTP/1.1"), "should contain request line")
assert(req_formatted:find("Host: example.com"), "should contain Host header")
assert(req_formatted:find('{"key":"value"}'), "should contain body")

-- test http.format_request with default method
local get_req = http.format_request({uri = "/test"})
assert(get_req:find("GET /test HTTP/1.1"), "should default to GET method")

-- test http.reason
assert(http.reason(200) == "OK", "reason(200) should be 'OK'")
assert(http.reason(404) == "Not Found", "reason(404) should be 'Not Found'")
assert(http.reason(500) == "Internal Server Error", "reason(500) should be 'Internal Server Error'")

-- test round-trip: parse then format
local original = "GET /path?query=1 HTTP/1.1\r\nHost: test.com\r\n\r\n"
local parsed = http.parse(original)
local reformatted = http.format_request({
  method = parsed.method,
  uri = parsed.uri,
  headers = parsed.headers
})
assert(reformatted:find("GET /path%?query=1 HTTP/1.1"), "round-trip should preserve request line")
assert(reformatted:find("Host: test.com"), "round-trip should preserve headers")

-- test http.serve exists
assert(type(http.serve) == "function", "http.serve should be a function")

print("all http tests passed")
