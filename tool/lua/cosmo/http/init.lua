---@meta cosmo.http

---HTTP parsing, formatting, and server module for Lua.
---
---This module provides low-level HTTP primitives for building servers and clients,
---plus a high-level serve() function for handler-based HTTP servers.
---
---Security features:
---- Header names/values are validated to prevent CRLF injection attacks
---- Status codes must be in valid HTTP range (100-599)
---- Maximum of 100 headers per request/response
---- Port numbers are validated (1-65535)
---- Timeout values must be non-negative
---- Content-Length is respected for request body reading
---
---@class cosmo.http
local http = {}

---Parsed HTTP request
---@class HttpRequest
---@field method string HTTP method (e.g., "GET", "POST", "PUT", "DELETE")
---@field uri string Request URI (e.g., "/path?query=value")
---@field version integer HTTP version (11 for HTTP/1.1, 10 for HTTP/1.0)
---@field headers table<string, string> Request headers
---@field body? string Request body (may be nil for GET requests)
---@field header_size integer Size of headers in bytes
---@field client_ip? string Client IP address (only in serve() handler)
---@field client_port? integer Client port (only in serve() handler)

---Parsed HTTP response
---@class HttpResponse
---@field status integer HTTP status code (e.g., 200, 404, 500)
---@field message string Status message (e.g., "OK", "Not Found")
---@field version integer HTTP version (11 for HTTP/1.1, 10 for HTTP/1.0)
---@field headers table<string, string> Response headers
---@field body? string Response body
---@field header_size integer Size of headers in bytes

---Response template for formatting or returning from handler
---@class HttpResponseTemplate
---@field status? integer HTTP status code (default: 200)
---@field headers? table<string, string> Response headers
---@field body? string Response body

---Request template for formatting
---@class HttpRequestTemplate
---@field method? string HTTP method (default: "GET")
---@field uri string Request URI (required)
---@field headers? table<string, string> Request headers
---@field body? string Request body

---Server options
---@class HttpServeOptions
---@field addr string Address to listen on (e.g., ":8080", "127.0.0.1:8080")
---@field reuseaddr? boolean Enable SO_REUSEADDR (default: true)
---@field backlog? integer Listen backlog (default: 128)
---@field timeout? integer Request timeout in seconds (default: 30)

---Parse an HTTP request from a raw string.
---
---Example:
---```lua
---local http = require("cosmo.http")
---local req = http.parse("GET /hello HTTP/1.1\r\nHost: localhost\r\n\r\n")
---print(req.method)  -- "GET"
---print(req.uri)     -- "/hello"
---```
---
---@param raw_request string Raw HTTP request string
---@return HttpRequest? request Parsed request, or nil on error
---@return string? error Error message if parsing failed
function http.parse(raw_request) end

---Parse an HTTP response from a raw string.
---
---Example:
---```lua
---local http = require("cosmo.http")
---local resp = http.parse_response("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello</h1>")
---print(resp.status)  -- 200
---print(resp.body)    -- "<h1>Hello</h1>"
---```
---
---@param raw_response string Raw HTTP response string
---@return HttpResponse? response Parsed response, or nil on error
---@return string? error Error message if parsing failed
function http.parse_response(raw_response) end

---Format an HTTP response from a table.
---
---Validates status code (must be 100-599) and header names/values.
---Raises an error if validation fails.
---
---Example:
---```lua
---local http = require("cosmo.http")
---local raw = http.format_response({
---    status = 200,
---    headers = {["Content-Type"] = "text/html"},
---    body = "<h1>Hello</h1>"
---})
---```
---
---@param response HttpResponseTemplate Response configuration
---@return string formatted_response HTTP response string
function http.format_response(response) end

---Format an HTTP request from a table.
---
---Validates method, URI, and header names/values.
---Raises an error if validation fails.
---
---Example:
---```lua
---local http = require("cosmo.http")
---local raw = http.format_request({
---    method = "POST",
---    uri = "/api/data",
---    headers = {Host = "example.com"},
---    body = '{"key": "value"}'
---})
---```
---
---@param request HttpRequestTemplate Request configuration
---@return string formatted_request HTTP request string
function http.format_request(request) end

---Get the standard reason phrase for an HTTP status code.
---
---Example:
---```lua
---local http = require("cosmo.http")
---print(http.reason(200))  -- "OK"
---print(http.reason(404))  -- "Not Found"
---```
---
---@param status_code integer HTTP status code (100-599)
---@return string reason Standard reason phrase
function http.reason(status_code) end

---Get the header name for a header constant.
---
---@param header_constant integer Header constant (e.g., http.HOST)
---@return string? header_name Header name, or nil if invalid
function http.header_name(header_constant) end

---Start an HTTP server with a handler function.
---
---The server runs in a blocking loop, calling the handler for each request.
---The handler receives a request table and should return a response table.
---
---Security: Invalid headers in the response are silently skipped. Invalid
---status codes are normalized to 500. Request bodies are read according to
---Content-Length header. Port numbers are validated (1-65535).
---
---Example:
---```lua
---local http = require("cosmo.http")
---
---http.serve(":8080", function(req)
---    return {
---        status = 200,
---        headers = {["Content-Type"] = "text/plain"},
---        body = "Hello, " .. req.uri
---    }
---end)
---```
---
---With options:
---```lua
---http.serve({
---    addr = ":8080",
---    timeout = 60,
---    backlog = 256
---}, function(req)
---    return {status = 200, body = "Hello"}
---end)
---```
---
---@param addr_or_options string|HttpServeOptions Address string or options table
---@param handler fun(request: HttpRequest): HttpResponseTemplate Handler function
function http.serve(addr_or_options, handler) end

-- HTTP method constants
---@type integer
http.GET = nil
---@type integer
http.POST = nil
---@type integer
http.PUT = nil
---@type integer
http.DELETE = nil
---@type integer
http.HEAD = nil
---@type integer
http.OPTIONS = nil
---@type integer
http.CONNECT = nil
---@type integer
http.TRACE = nil

-- Status code constants
---@type integer
http.OK = 200
---@type integer
http.CREATED = 201
---@type integer
http.NO_CONTENT = 204
---@type integer
http.MOVED_PERMANENTLY = 301
---@type integer
http.FOUND = 302
---@type integer
http.NOT_MODIFIED = 304
---@type integer
http.BAD_REQUEST = 400
---@type integer
http.UNAUTHORIZED = 401
---@type integer
http.FORBIDDEN = 403
---@type integer
http.NOT_FOUND = 404
---@type integer
http.METHOD_NOT_ALLOWED = 405
---@type integer
http.INTERNAL_SERVER_ERROR = 500
---@type integer
http.BAD_GATEWAY = 502
---@type integer
http.SERVICE_UNAVAILABLE = 503

-- Header constants
---@type integer
http.HOST = nil
---@type integer
http.CONTENT_TYPE = nil
---@type integer
http.CONTENT_LENGTH = nil
---@type integer
http.CONNECTION = nil
---@type integer
http.ACCEPT = nil
---@type integer
http.USER_AGENT = nil

return http
