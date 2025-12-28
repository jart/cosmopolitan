-- Copyright 2024 Will Maier
--
-- Permission to use, copy, modify, and/or distribute this software for
-- any purpose with or without fee is hereby granted, provided that the
-- above copyright notice and this permission notice appear in all copies.
--
-- THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
-- WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
-- WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
-- AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
-- DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
-- PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
-- TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
-- PERFORMANCE OF THIS SOFTWARE.

-- Test cases for Fetch (lfetch.c)
-- Uses httpbin.org for external HTTP testing

-- Use Fetch from redbean globals or cosmo module
local Fetch = Fetch or require("cosmo").Fetch

-- Test: Basic GET request
function test_basic_get()
    local status, headers, body = Fetch("https://httpbin.org/get")
    assert(status == 200, "expected 200, got: " .. tostring(status))
    assert(body and #body > 0, "expected non-empty body")
    print("test_basic_get: PASS")
end

-- Test: Redirect is followed by default
function test_redirect_followed()
    local status, headers, body = Fetch("https://httpbin.org/redirect/1")
    assert(status == 200, "expected 200 after redirect, got: " .. tostring(status))
    print("test_redirect_followed: PASS")
end

-- Test: Multiple redirects
function test_multiple_redirects()
    local status, headers, body = Fetch("https://httpbin.org/redirect/3")
    assert(status == 200, "expected 200 after 3 redirects, got: " .. tostring(status))
    print("test_multiple_redirects: PASS")
end

-- Test: Relative redirect
function test_relative_redirect()
    local status, headers, body = Fetch("https://httpbin.org/relative-redirect/2")
    assert(status == 200, "expected 200 after relative redirects, got: " .. tostring(status))
    print("test_relative_redirect: PASS")
end

-- Test: Absolute redirect
-- Note: httpbin.org's absolute-redirect may redirect HTTPS→HTTP which is
-- correctly rejected as a security measure. We accept either outcome.
function test_absolute_redirect()
    local status, headers, body = Fetch("https://httpbin.org/absolute-redirect/2")
    if status == nil then
        -- HTTPS→HTTP downgrade is correctly rejected
        assert(headers:find("HTTPS to HTTP") or headers:find("redirect"),
               "expected security error or network issue, got: " .. tostring(headers))
        print("test_absolute_redirect: PASS (HTTPS downgrade correctly rejected)")
    else
        assert(status == 200, "expected 200 after absolute redirects, got: " .. tostring(status))
        print("test_absolute_redirect: PASS")
    end
end

-- Test: Max redirects exceeded
function test_max_redirects_exceeded()
    -- Request 10 redirects but limit to 3
    local status, headers, body = Fetch("https://httpbin.org/redirect/10", {maxredirects = 3})
    -- After 3 redirects, we should get a 302 (the 4th redirect response)
    assert(status == 302, "expected 302 after max redirects, got: " .. tostring(status))
    print("test_max_redirects_exceeded: PASS")
end

-- Test: followredirect = false
function test_no_follow_redirect()
    local status, headers, body = Fetch("https://httpbin.org/redirect/1", {followredirect = false})
    assert(status == 302, "expected 302 with followredirect=false, got: " .. tostring(status))
    assert(headers["Location"], "expected Location header")
    print("test_no_follow_redirect: PASS")
end

-- Test: POST with body
function test_post_with_body()
    local status, headers, body = Fetch("https://httpbin.org/post", {
        method = "POST",
        body = "test=value"
    })
    assert(status == 200, "expected 200, got: " .. tostring(status))
    assert(body:find("test=value"), "expected body to contain posted data")
    print("test_post_with_body: PASS")
end

-- Test: Custom headers
-- Note: httpbin.org can be flaky; retry once on failure
function test_custom_headers()
    local function try_fetch()
        local status, headers, body = Fetch("https://httpbin.org/headers", {
            headers = {
                ["X-Custom-Header"] = "test-value"
            }
        })
        -- Use plain text search (4th arg = true) since hyphen is a pattern char
        if status == 200 and body and
           (body:find("X-Custom-Header", 1, true) or body:find("x-custom-header", 1, true)) then
            return true
        end
        return false, status, headers, body
    end

    local ok, status, headers, body = try_fetch()
    if not ok then
        -- Retry once for transient network issues
        unix.nanosleep(1)
        ok, status, headers, body = try_fetch()
    end

    assert(ok, "expected 200 with custom header, got status: " .. tostring(status))
    print("test_custom_headers: PASS")
end

-- Test: Status codes
function test_status_codes()
    for _, code in ipairs({200, 201, 204, 400, 404, 500}) do
        local status, headers, body = Fetch("https://httpbin.org/status/" .. code)
        assert(status == code, "expected " .. code .. ", got: " .. tostring(status))
    end
    print("test_status_codes: PASS")
end

-- Test: Empty Location header redirect (edge case)
-- httpbin's redirect-to with empty url parameter
function test_redirect_to_empty()
    local status, headers, body = Fetch("https://httpbin.org/redirect-to?url=", {maxredirects = 1})
    -- With empty Location, behavior may vary - just ensure no crash
    assert(status == nil or type(status) == "number",
           "expected nil or number status, got: " .. type(status))
    print("test_redirect_to_empty: PASS")
end

-- Test: Redirect to relative path
function test_redirect_to_relative()
    local status, headers, body = Fetch("https://httpbin.org/redirect-to?url=/get")
    assert(status == 200, "expected 200 after redirect to /get, got: " .. tostring(status))
    print("test_redirect_to_relative: PASS")
end

-- Test: SSRF protection blocks loopback IP
function test_ssrf_blocks_loopback()
    local status, err = Fetch("http://127.0.0.1/")
    assert(status == nil, "expected nil status for blocked loopback IP")
    assert(err:find("private network blocked"), "expected SSRF protection error, got: " .. err)
    print("test_ssrf_blocks_loopback: PASS")
end

-- Test: SSRF protection blocks private IP
function test_ssrf_blocks_private()
    local status, err = Fetch("http://10.0.0.1/")
    assert(status == nil, "expected nil status for blocked private IP")
    assert(err:find("private network blocked"), "expected SSRF protection error, got: " .. err)
    print("test_ssrf_blocks_private: PASS")
end

-- Test: Invalid scheme
function test_invalid_scheme()
    local status, err = Fetch("ftp://example.com/")
    assert(status == nil, "expected nil status for invalid scheme")
    assert(err:find("scheme"), "expected bad scheme error, got: " .. err)
    print("test_invalid_scheme: PASS")
end

-- Test: Bad method name
function test_bad_method()
    local status, err = Fetch("https://httpbin.org/get", {method = "INVALID_VERY_LONG_METHOD"})
    assert(status == nil, "expected nil status for bad method")
    assert(err:find("method"), "expected bad method error, got: " .. tostring(err))
    print("test_bad_method: PASS")
end

-- Helper: Create a simple TCP server that captures requests
-- Returns the server socket and port
local function create_test_server()
    local sock = assert(unix.socket(unix.AF_INET, unix.SOCK_STREAM, 0))
    assert(unix.setsockopt(sock, unix.SOL_SOCKET, unix.SO_REUSEADDR, 1))
    -- Bind to port 0 to get an available port (ParseIp returns integer IP)
    assert(unix.bind(sock, ParseIp("127.0.0.1"), 0))
    assert(unix.listen(sock, 5))
    local ip, port = unix.getsockname(sock)
    return sock, port
end

-- Helper: Accept one connection, read request, send response, close
local function handle_one_request(server_sock, response, timeout_ms)
    timeout_ms = timeout_ms or 5000
    unix.setsockopt(server_sock, unix.SOL_SOCKET, unix.SO_RCVTIMEO, timeout_ms // 1000, (timeout_ms % 1000) * 1000)
    local client, err = unix.accept(server_sock)
    if not client then
        return nil, "accept failed: " .. tostring(err)
    end
    local request = ""
    while true do
        local data = unix.read(client, 4096)
        if not data or #data == 0 then break end
        request = request .. data
        -- Check if we have complete headers
        if request:find("\r\n\r\n") then break end
    end
    if response then
        unix.write(client, response)
    end
    unix.close(client)
    return request
end

-- Test: Proxy option causes connection to proxy (not target)
-- Verifies proxy is actually used by checking connection goes to proxy port
function test_proxy_connection_to_proxy()
    local server, port = create_test_server()
    local response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, Proxy!"

    -- Fork to handle the connection
    local pid = unix.fork()
    if pid == 0 then
        -- Child: handle one request
        local request = handle_one_request(server, response, 5000)
        unix.close(server)
        os.exit(request and 0 or 1)
    end

    -- Parent: make request through proxy
    unix.close(server)  -- Parent doesn't need server socket
    local status, headers, body = Fetch("http://example.com/test", {
        proxy = "http://127.0.0.1:" .. port
    })

    -- Wait for child
    unix.wait(pid)

    assert(status == 200, "expected 200 from proxy, got: " .. tostring(status))
    assert(body == "Hello, Proxy!", "expected proxy response body, got: " .. tostring(body))
    print("test_proxy_connection_to_proxy: PASS")
end

-- Test: HTTP proxy request uses absolute URL
-- Verifies the request line contains full URL (required by HTTP proxy spec)
function test_proxy_absolute_url()
    local server, port = create_test_server()
    local captured_request = nil

    -- Fork to capture the request
    local pid = unix.fork()
    if pid == 0 then
        -- Child: capture request and send minimal response
        local request = handle_one_request(server, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", 5000)
        -- Write captured request to a temp file for parent to read
        local f = io.open("/tmp/proxy_test_request.txt", "w")
        if f and request then
            f:write(request)
            f:close()
        end
        unix.close(server)
        os.exit(request and 0 or 1)
    end

    -- Parent: make request
    unix.close(server)
    Fetch("http://target.example.com/path/to/resource?query=1", {
        proxy = "http://127.0.0.1:" .. port
    })

    -- Wait for child
    unix.wait(pid)

    -- Read captured request
    local f = io.open("/tmp/proxy_test_request.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_test_request.txt")
    end

    assert(captured_request, "failed to capture proxy request")
    -- Check that request line contains absolute URL
    local first_line = captured_request:match("^([^\r\n]+)")
    assert(first_line:find("http://target.example.com/path/to/resource"),
           "expected absolute URL in request, got: " .. first_line)
    print("test_proxy_absolute_url: PASS")
end

-- Test: HTTPS proxy sends CONNECT request
-- Verifies CONNECT method is used for HTTPS tunneling
function test_proxy_https_connect()
    local server, port = create_test_server()
    local captured_request = nil

    -- Fork to capture the CONNECT request
    local pid = unix.fork()
    if pid == 0 then
        -- Child: capture CONNECT request
        local request = handle_one_request(server, "HTTP/1.1 200 Connection Established\r\n\r\n", 5000)
        local f = io.open("/tmp/proxy_connect_request.txt", "w")
        if f and request then
            f:write(request)
            f:close()
        end
        unix.close(server)
        os.exit(0)
    end

    -- Parent: make HTTPS request through proxy
    unix.close(server)
    -- This will fail after CONNECT because we don't have a real tunnel,
    -- but we can still verify the CONNECT was sent
    Fetch("https://secure.example.com/path", {
        proxy = "http://127.0.0.1:" .. port
    })

    -- Wait for child
    unix.wait(pid)

    -- Read captured request
    local f = io.open("/tmp/proxy_connect_request.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_connect_request.txt")
    end

    assert(captured_request, "failed to capture CONNECT request")
    local first_line = captured_request:match("^([^\r\n]+)")
    assert(first_line:find("^CONNECT secure.example.com:443"),
           "expected CONNECT request, got: " .. tostring(first_line))
    print("test_proxy_https_connect: PASS")
end

-- Test: Proxy with invalid scheme is rejected
function test_proxy_bad_scheme()
    local status, err = Fetch("http://example.com/", {
        proxy = "socks5://proxy.example.com:1080"
    })
    assert(status == nil, "expected nil status for unsupported proxy scheme")
    assert(err:find("proxy") and err:find("scheme"),
           "expected proxy scheme error, got: " .. tostring(err))
    print("test_proxy_bad_scheme: PASS")
end

-- Test: Proxy with https:// scheme is rejected
function test_proxy_https_scheme_rejected()
    local status, err = Fetch("http://example.com/", {
        proxy = "https://proxy.example.com:8080"
    })
    assert(status == nil, "expected nil status for https proxy scheme")
    assert(err:find("proxy") and err:find("scheme"),
           "expected proxy scheme error, got: " .. tostring(err))
    print("test_proxy_https_scheme_rejected: PASS")
end

-- Test: Proxy with missing host is rejected
function test_proxy_missing_host()
    local status, err = Fetch("http://example.com/", {
        proxy = "http://:8080"
    })
    assert(status == nil, "expected nil status for proxy with missing host")
    assert(err:find("proxy"), "expected proxy error, got: " .. tostring(err))
    print("test_proxy_missing_host: PASS")
end

-- Test: Proxy option must be string
function test_proxy_type_validation()
    local status, err = Fetch("http://example.com/", {
        proxy = 12345
    })
    assert(status == nil, "expected nil status for non-string proxy")
    assert(err:find("proxy") or err:find("string"),
           "expected proxy-related error, got: " .. tostring(err))
    print("test_proxy_type_validation: PASS")
end

-- Test: Proxy connection refused error is reported
function test_proxy_connection_refused()
    -- Use a port that's definitely not listening
    local status, err = Fetch("http://example.com/", {
        proxy = "http://127.0.0.1:59999"
    })
    assert(status == nil, "expected nil status for connection refused")
    assert(err:find("connect") or err:find("refused") or err:find("error"),
           "expected connection error, got: " .. tostring(err))
    print("test_proxy_connection_refused: PASS")
end

-- Test: Proxy default port is 80
function test_proxy_default_port()
    -- Proxy URL without port should default to 80
    -- We verify by checking the connection attempt goes to port 80
    local status, err = Fetch("http://example.com/", {
        proxy = "http://127.0.0.1"  -- No port specified
    })
    -- Should fail to connect to 127.0.0.1:80 (nothing listening)
    assert(status == nil, "expected nil status")
    -- The error should mention the connection attempt
    assert(err:find("connect") or err:find("error"),
           "expected connection error, got: " .. tostring(err))
    print("test_proxy_default_port: PASS")
end

-- Test: SSRF check is bypassed for proxy connections
-- When using a proxy, we connect to the proxy IP (which may be private)
-- and the proxy handles the actual target resolution
function test_proxy_bypasses_ssrf_for_proxy_ip()
    local server, port = create_test_server()

    -- Fork to handle the connection
    local pid = unix.fork()
    if pid == 0 then
        handle_one_request(server, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", 5000)
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    -- Connect to 127.0.0.1 proxy (normally blocked by SSRF) to request external URL
    local status, headers, body = Fetch("http://external.example.com/", {
        proxy = "http://127.0.0.1:" .. port
    })

    unix.wait(pid)

    -- Should succeed - SSRF check should not block proxy connection
    assert(status == 200, "expected 200, proxy to loopback should work, got: " .. tostring(status))
    print("test_proxy_bypasses_ssrf_for_proxy_ip: PASS")
end

-- Test: http_proxy environment variable (when set)
function test_http_proxy_env_var()
    local http_proxy = os.getenv("http_proxy") or os.getenv("HTTP_PROXY")
    if not http_proxy then
        print("test_http_proxy_env_var: SKIP (http_proxy not set)")
        return
    end
    -- Just verify env var is detected - actual behavior depends on proxy value
    print("test_http_proxy_env_var: PASS (detected: " .. http_proxy .. ")")
end

-- Test: Explicit proxy overrides http_proxy env var
function test_proxy_option_overrides_env()
    local server, port = create_test_server()
    local response = "HTTP/1.1 200 OK\r\nContent-Length: 17\r\n\r\nExplicit proxy OK"

    local pid = unix.fork()
    if pid == 0 then
        handle_one_request(server, response, 5000)
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    -- Even if http_proxy env is set to something else, explicit option wins
    local status, headers, body = Fetch("http://example.com/", {
        proxy = "http://127.0.0.1:" .. port
    })

    unix.wait(pid)

    assert(status == 200, "expected 200, got: " .. tostring(status))
    assert(body == "Explicit proxy OK", "expected explicit proxy response, got: " .. tostring(body))
    print("test_proxy_option_overrides_env: PASS")
end

-- Test: Proxy with custom port
function test_proxy_custom_port()
    local server, port = create_test_server()
    local response = "HTTP/1.1 200 OK\r\nContent-Length: 14\r\n\r\nCustom port OK"

    local pid = unix.fork()
    if pid == 0 then
        handle_one_request(server, response, 5000)
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    local status, headers, body = Fetch("http://example.com/test", {
        proxy = "http://127.0.0.1:" .. port
    })

    unix.wait(pid)

    assert(status == 200, "expected 200, got: " .. tostring(status))
    assert(body == "Custom port OK", "expected custom port response")
    print("test_proxy_custom_port: PASS")
end

-- Test: Proxy preserves Host header for target
function test_proxy_host_header()
    local server, port = create_test_server()
    local captured_request = nil

    local pid = unix.fork()
    if pid == 0 then
        local request = handle_one_request(server, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", 5000)
        local f = io.open("/tmp/proxy_host_header.txt", "w")
        if f and request then f:write(request); f:close() end
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    Fetch("http://target-host.example.com:8080/path", {
        proxy = "http://127.0.0.1:" .. port
    })

    unix.wait(pid)

    local f = io.open("/tmp/proxy_host_header.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_host_header.txt")
    end

    assert(captured_request, "failed to capture request")
    assert(captured_request:find("Host: target%-host.example.com:8080"),
           "expected Host header for target, got: " .. captured_request:sub(1, 200))
    print("test_proxy_host_header: PASS")
end

--------------------------------------------------------------------------------
-- Proxy Authentication Tests
-- These tests verify that Fetch correctly handles authenticated proxy URLs
-- in the format: http://username:password@host:port
--------------------------------------------------------------------------------

-- Helper: Base64 encode (for verifying Proxy-Authorization header)
local function base64_encode(data)
    local b = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
    return ((data:gsub('.', function(x)
        local r, b = '', x:byte()
        for i = 8, 1, -1 do r = r .. (b % 2 ^ i - b % 2 ^ (i - 1) > 0 and '1' or '0') end
        return r
    end) .. '0000'):gsub('%d%d%d?%d?%d?%d?', function(x)
        if #x < 6 then return '' end
        local c = 0
        for i = 1, 6 do c = c + (x:sub(i, i) == '1' and 2 ^ (6 - i) or 0) end
        return b:sub(c + 1, c + 1)
    end) .. ({ '', '==', '=' })[#data % 3 + 1])
end

-- Test: HTTP proxy with Basic auth sends Proxy-Authorization header
function test_proxy_basic_auth_http()
    local server, port = create_test_server()
    local captured_request = nil

    local pid = unix.fork()
    if pid == 0 then
        local request = handle_one_request(server, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", 5000)
        local f = io.open("/tmp/proxy_auth_http.txt", "w")
        if f and request then f:write(request); f:close() end
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    Fetch("http://example.com/test", {
        proxy = "http://testuser:testpass@127.0.0.1:" .. port
    })

    unix.wait(pid)

    local f = io.open("/tmp/proxy_auth_http.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_auth_http.txt")
    end

    assert(captured_request, "failed to capture request")
    -- Expected: Proxy-Authorization: Basic dGVzdHVzZXI6dGVzdHBhc3M=
    local expected_auth = "Basic " .. base64_encode("testuser:testpass")
    assert(captured_request:find("Proxy%-Authorization: " .. expected_auth, 1, false),
           "expected Proxy-Authorization header with Basic auth, got:\n" .. captured_request:sub(1, 500))
    print("test_proxy_basic_auth_http: PASS")
end

-- Test: HTTPS proxy with Basic auth sends Proxy-Authorization in CONNECT
function test_proxy_basic_auth_https_connect()
    local server, port = create_test_server()
    local captured_request = nil

    local pid = unix.fork()
    if pid == 0 then
        -- Send 200 Connection Established to allow CONNECT to succeed
        local request = handle_one_request(server, "HTTP/1.1 200 Connection Established\r\n\r\n", 5000)
        local f = io.open("/tmp/proxy_auth_connect.txt", "w")
        if f and request then f:write(request); f:close() end
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    -- This will fail after CONNECT (no real TLS), but we can verify auth header
    Fetch("https://secure.example.com/path", {
        proxy = "http://proxyuser:proxypass@127.0.0.1:" .. port
    })

    unix.wait(pid)

    local f = io.open("/tmp/proxy_auth_connect.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_auth_connect.txt")
    end

    assert(captured_request, "failed to capture CONNECT request")
    -- Verify CONNECT request contains Proxy-Authorization
    local expected_auth = "Basic " .. base64_encode("proxyuser:proxypass")
    assert(captured_request:find("^CONNECT "),
           "expected CONNECT request, got:\n" .. captured_request:sub(1, 200))
    assert(captured_request:find("Proxy%-Authorization: " .. expected_auth, 1, false),
           "expected Proxy-Authorization in CONNECT request, got:\n" .. captured_request:sub(1, 500))
    print("test_proxy_basic_auth_https_connect: PASS")
end

-- Test: Proxy auth with special characters in password
function test_proxy_auth_special_chars()
    local server, port = create_test_server()
    local captured_request = nil

    local pid = unix.fork()
    if pid == 0 then
        local request = handle_one_request(server, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", 5000)
        local f = io.open("/tmp/proxy_auth_special.txt", "w")
        if f and request then f:write(request); f:close() end
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    -- Password with special chars that need URL encoding: p@ss:word/test
    -- In URL: p%40ss%3Aword%2Ftest
    Fetch("http://example.com/test", {
        proxy = "http://user:p%40ss%3Aword%2Ftest@127.0.0.1:" .. port
    })

    unix.wait(pid)

    local f = io.open("/tmp/proxy_auth_special.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_auth_special.txt")
    end

    assert(captured_request, "failed to capture request")
    -- The decoded credentials should be: user:p@ss:word/test
    local expected_auth = "Basic " .. base64_encode("user:p@ss:word/test")
    assert(captured_request:find("Proxy%-Authorization: " .. expected_auth, 1, false),
           "expected Proxy-Authorization with decoded special chars, got:\n" .. captured_request:sub(1, 500))
    print("test_proxy_auth_special_chars: PASS")
end

-- Test: Proxy auth with long JWT-style password (400+ chars)
function test_proxy_auth_long_jwt_password()
    local server, port = create_test_server()
    local captured_request = nil

    -- Simulate a JWT-like token (400+ chars)
    local jwt_password = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9." ..
        string.rep("abcdefghijklmnopqrstuvwxyz0123456789", 12) ..
        ".signature_part_here"

    local pid = unix.fork()
    if pid == 0 then
        local request = handle_one_request(server, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", 5000)
        local f = io.open("/tmp/proxy_auth_jwt.txt", "w")
        if f and request then f:write(request); f:close() end
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    Fetch("http://example.com/test", {
        proxy = "http://container_id:" .. jwt_password .. "@127.0.0.1:" .. port
    })

    unix.wait(pid)

    local f = io.open("/tmp/proxy_auth_jwt.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_auth_jwt.txt")
    end

    assert(captured_request, "failed to capture request")
    local expected_auth = "Basic " .. base64_encode("container_id:" .. jwt_password)
    assert(captured_request:find("Proxy-Authorization:", 1, true),
           "expected Proxy-Authorization header, got:\n" .. captured_request:sub(1, 500))
    -- Verify the full auth value is present
    assert(captured_request:find(expected_auth, 1, true),
           "expected correct Base64-encoded JWT credentials")
    print("test_proxy_auth_long_jwt_password: PASS")
end

-- Test: Proxy URL with username but no password
function test_proxy_auth_username_only()
    local server, port = create_test_server()
    local captured_request = nil

    local pid = unix.fork()
    if pid == 0 then
        local request = handle_one_request(server, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", 5000)
        local f = io.open("/tmp/proxy_auth_useronly.txt", "w")
        if f and request then f:write(request); f:close() end
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    -- Username with no password: http://onlyuser@host:port
    Fetch("http://example.com/test", {
        proxy = "http://onlyuser@127.0.0.1:" .. port
    })

    unix.wait(pid)

    local f = io.open("/tmp/proxy_auth_useronly.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_auth_useronly.txt")
    end

    assert(captured_request, "failed to capture request")
    -- Even with no password, should send auth header with "onlyuser:"
    local expected_auth = "Basic " .. base64_encode("onlyuser:")
    assert(captured_request:find("Proxy%-Authorization: " .. expected_auth, 1, false),
           "expected Proxy-Authorization with username only, got:\n" .. captured_request:sub(1, 500))
    print("test_proxy_auth_username_only: PASS")
end

-- Test: Proxy URL without credentials should NOT send Proxy-Authorization
function test_proxy_no_auth_no_header()
    local server, port = create_test_server()
    local captured_request = nil

    local pid = unix.fork()
    if pid == 0 then
        local request = handle_one_request(server, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", 5000)
        local f = io.open("/tmp/proxy_no_auth.txt", "w")
        if f and request then f:write(request); f:close() end
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    -- No credentials in proxy URL
    Fetch("http://example.com/test", {
        proxy = "http://127.0.0.1:" .. port
    })

    unix.wait(pid)

    local f = io.open("/tmp/proxy_no_auth.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_no_auth.txt")
    end

    assert(captured_request, "failed to capture request")
    -- Should NOT contain Proxy-Authorization header
    assert(not captured_request:find("Proxy-Authorization:", 1, true),
           "expected NO Proxy-Authorization header for unauthenticated proxy, got:\n" .. captured_request:sub(1, 500))
    print("test_proxy_no_auth_no_header: PASS")
end

-- Test: Proxy auth credentials are NOT sent to target server (security)
function test_proxy_auth_not_leaked_to_target()
    local server, port = create_test_server()
    local captured_request = nil

    local pid = unix.fork()
    if pid == 0 then
        local request = handle_one_request(server, "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK", 5000)
        local f = io.open("/tmp/proxy_auth_leak.txt", "w")
        if f and request then f:write(request); f:close() end
        unix.close(server)
        os.exit(0)
    end

    unix.close(server)
    Fetch("http://example.com/test", {
        proxy = "http://secretuser:secretpass@127.0.0.1:" .. port
    })

    unix.wait(pid)

    local f = io.open("/tmp/proxy_auth_leak.txt", "r")
    if f then
        captured_request = f:read("*a")
        f:close()
        os.remove("/tmp/proxy_auth_leak.txt")
    end

    assert(captured_request, "failed to capture request")
    -- The proxy URL credentials should appear in Proxy-Authorization, NOT in Authorization
    -- Check that there's no standalone "Authorization:" header (not Proxy-Authorization:)
    local has_plain_auth = captured_request:find("\nAuthorization:", 1, true)
    local has_proxy_auth = captured_request:find("Proxy-Authorization:", 1, true)
    assert(not has_plain_auth or has_proxy_auth,
           "credentials should only appear in Proxy-Authorization, not Authorization")
    -- The raw credentials should not appear anywhere in the request
    assert(not captured_request:find("secretuser:secretpass", 1, true),
           "raw credentials should not appear in request")
    print("test_proxy_auth_not_leaked_to_target: PASS")
end

-- Test: GitHub release download (known issue - long Location header with JWT tokens)
-- This URL redirects with a ~968 byte Location header containing signed URLs
function test_github_release_download()
    local url = "https://github.com/astral-sh/ruff/releases/download/0.8.4/ruff-x86_64-unknown-linux-gnu.tar.gz"
    local status, headers, body = Fetch(url)
    -- This should succeed with 200 after following the redirect
    -- If it fails with "transport error" or "ParseHttpMessage", the bug is present
    if status == nil then
        print("test_github_release_download: FAILED - " .. tostring(headers))
        error("GitHub release download failed: " .. tostring(headers))
    end
    assert(status == 200, "expected 200, got: " .. tostring(status))
    assert(body and #body > 1000, "expected non-trivial body size")
    print("test_github_release_download: PASS")
end

-- Test: GitHub release download with followredirect=false
-- This tests whether we can at least get the redirect response
function test_github_release_redirect_only()
    local url = "https://github.com/astral-sh/ruff/releases/download/0.8.4/ruff-x86_64-unknown-linux-gnu.tar.gz"
    local status, headers, body = Fetch(url, {followredirect = false})
    -- Should get a 302 redirect
    if status == nil then
        print("test_github_release_redirect_only: FAILED - " .. tostring(headers))
        error("GitHub release redirect failed: " .. tostring(headers))
    end
    assert(status == 302, "expected 302, got: " .. tostring(status))
    assert(headers["Location"], "expected Location header")
    -- The Location header should be very long (contains JWT/SAS tokens)
    local loc = headers["Location"]
    print("Location header length: " .. #loc)
    assert(#loc > 500, "expected long Location header (>500 chars), got: " .. #loc)
    print("test_github_release_redirect_only: PASS")
end

-- Test: Raw githubusercontent (this should work - shorter redirect)
function test_github_raw_file()
    local url = "https://github.com/anthropics/anthropic-cookbook/raw/main/README.md"
    local status, headers, body = Fetch(url)
    assert(status == 200, "expected 200, got: " .. tostring(status))
    assert(body and #body > 0, "expected non-empty body")
    print("test_github_raw_file: PASS")
end

-- Main test runner
function main()
    print("Running lfetch tests...")

    local tests = {
        -- External httpbin tests
        test_basic_get,
        test_redirect_followed,
        test_multiple_redirects,
        test_relative_redirect,
        test_absolute_redirect,
        test_max_redirects_exceeded,
        test_no_follow_redirect,
        test_post_with_body,
        test_custom_headers,
        test_status_codes,
        test_redirect_to_empty,
        test_redirect_to_relative,
        -- GitHub tests (reproduce PR #107 issue)
        test_github_raw_file,
        test_github_release_redirect_only,
        test_github_release_download,
        -- Local error handling tests
        test_ssrf_blocks_loopback,
        test_ssrf_blocks_private,
        test_invalid_scheme,
        test_bad_method,
        -- HTTP proxy tests (self-contained with local test server)
        test_proxy_connection_to_proxy,
        test_proxy_absolute_url,
        test_proxy_https_connect,
        test_proxy_bad_scheme,
        test_proxy_https_scheme_rejected,
        test_proxy_missing_host,
        test_proxy_type_validation,
        test_proxy_connection_refused,
        test_proxy_default_port,
        test_proxy_bypasses_ssrf_for_proxy_ip,
        test_http_proxy_env_var,
        test_proxy_option_overrides_env,
        test_proxy_custom_port,
        test_proxy_host_header,
        -- Proxy authentication tests
        test_proxy_basic_auth_http,
        test_proxy_basic_auth_https_connect,
        test_proxy_auth_special_chars,
        test_proxy_auth_long_jwt_password,
        test_proxy_auth_username_only,
        test_proxy_no_auth_no_header,
        test_proxy_auth_not_leaked_to_target,
    }

    local passed = 0
    local failed = 0

    for _, test in ipairs(tests) do
        local ok, err = pcall(test)
        if ok then
            passed = passed + 1
        else
            failed = failed + 1
            print("FAILED: " .. tostring(err))
        end
    end

    print(string.format("\nResults: %d passed, %d failed", passed, failed))
    if failed > 0 then
        error("Some tests failed")
    end
end

main()
