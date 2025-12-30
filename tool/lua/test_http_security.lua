-- Security tests for cosmo.http module

local http = require("cosmo.http")

local function test_header_injection_response()
  -- Test: CRLF injection in response header names should be rejected
  local ok, err = pcall(function()
    http.format_response({
      status = 200,
      headers = {["X-Bad\r\nX-Injected"] = "value"},
      body = "test"
    })
  end)
  assert(not ok, "should reject header name with CRLF")
  assert(err:find("invalid header name"), "error should mention invalid header name")
end

local function test_header_injection_value()
  -- Test: CRLF injection in response header values should be rejected
  local ok, err = pcall(function()
    http.format_response({
      status = 200,
      headers = {["X-Good"] = "value\r\nX-Injected: evil"},
      body = "test"
    })
  end)
  assert(not ok, "should reject header value with CRLF")
  assert(err:find("invalid header value"), "error should mention invalid header value")
end

local function test_header_injection_request()
  -- Test: CRLF injection in request header names should be rejected
  local ok, err = pcall(function()
    http.format_request({
      method = "GET",
      uri = "/test",
      headers = {["X-Bad\r\nX-Injected"] = "value"}
    })
  end)
  assert(not ok, "should reject header name with CRLF in request")
  assert(err:find("invalid header name"), "error should mention invalid header name")
end

local function test_header_injection_request_value()
  -- Test: CRLF injection in request header values should be rejected
  local ok, err = pcall(function()
    http.format_request({
      method = "GET",
      uri = "/test",
      headers = {["X-Good"] = "value\r\nX-Injected: evil"}
    })
  end)
  assert(not ok, "should reject header value with CRLF in request")
  assert(err:find("invalid header value"), "error should mention invalid header value")
end

local function test_uri_injection()
  -- Test: CRLF injection in URI should be rejected
  local ok, err = pcall(function()
    http.format_request({
      method = "GET",
      uri = "/test\r\nX-Injected: evil"
    })
  end)
  assert(not ok, "should reject URI with CRLF")
  assert(err:find("invalid URI"), "error should mention invalid URI")
end

local function test_method_injection()
  -- Test: Invalid characters in method should be rejected
  local ok, err = pcall(function()
    http.format_request({
      method = "GET\r\nX-Injected: evil",
      uri = "/test"
    })
  end)
  assert(not ok, "should reject method with CRLF")
  assert(err:find("invalid HTTP method"), "error should mention invalid method")
end

local function test_status_code_validation()
  -- Test: Invalid status codes should be rejected
  local ok, err = pcall(function()
    http.format_response({status = 99})
  end)
  assert(not ok, "should reject status code 99")
  assert(err:find("invalid HTTP status"), "error should mention invalid status")

  local ok2, err2 = pcall(function()
    http.format_response({status = 600})
  end)
  assert(not ok2, "should reject status code 600")
  assert(err2:find("invalid HTTP status"), "error should mention invalid status")

  local ok3, err3 = pcall(function()
    http.format_response({status = -1})
  end)
  assert(not ok3, "should reject status code -1")
  assert(err3:find("invalid HTTP status"), "error should mention invalid status")
end

local function test_valid_status_codes()
  -- Test: Valid status codes should work
  local resp100 = http.format_response({status = 100})
  assert(resp100:find("HTTP/1.1 100"), "status 100 should work")

  local resp599 = http.format_response({status = 599})
  assert(resp599:find("HTTP/1.1 599"), "status 599 should work")

  local resp200 = http.format_response({status = 200})
  assert(resp200:find("HTTP/1.1 200 OK"), "status 200 should work")
end

local function test_control_char_rejection()
  -- Test: Control characters (except tab) should be rejected in header values
  local ok, err = pcall(function()
    http.format_response({
      status = 200,
      headers = {["X-Test"] = "value\x00null"},
      body = "test"
    })
  end)
  assert(not ok, "should reject header value with null byte")

  local ok2, err2 = pcall(function()
    http.format_response({
      status = 200,
      headers = {["X-Test"] = "value\x1fcontrol"},
      body = "test"
    })
  end)
  assert(not ok2, "should reject header value with control char")
end

local function test_tab_in_header_allowed()
  -- Test: Tab character should be allowed in header values (per HTTP spec)
  local ok, resp = pcall(function()
    return http.format_response({
      status = 200,
      headers = {["X-Test"] = "value\twith\ttabs"},
      body = "test"
    })
  end)
  assert(ok, "should allow tab in header value")
  assert(resp:find("value\twith\ttabs"), "tab should be preserved")
end

local function test_empty_header_name_rejected()
  -- Test: Empty header names should be rejected
  local ok, err = pcall(function()
    http.format_response({
      status = 200,
      headers = {[""] = "value"},
      body = "test"
    })
  end)
  assert(not ok, "should reject empty header name")
end

local function test_header_count_limit()
  -- Test: Too many headers should be rejected
  local headers = {}
  for i = 1, 150 do
    headers["X-Header-" .. i] = "value" .. i
  end
  local ok, err = pcall(function()
    http.format_response({
      status = 200,
      headers = headers,
      body = "test"
    })
  end)
  assert(not ok, "should reject too many headers")
  assert(err:find("too many headers"), "error should mention too many headers")
end

local function test_special_chars_in_token()
  -- Test: Invalid token characters in header name should be rejected
  local ok, err = pcall(function()
    http.format_response({
      status = 200,
      headers = {["X-Bad Header"] = "value"},  -- space not allowed in token
      body = "test"
    })
  end)
  assert(not ok, "should reject header name with space")
end

-- Run all tests
print("Running security tests...")

test_header_injection_response()
print("  [PASS] Header injection in response name prevented")

test_header_injection_value()
print("  [PASS] Header injection in response value prevented")

test_header_injection_request()
print("  [PASS] Header injection in request name prevented")

test_header_injection_request_value()
print("  [PASS] Header injection in request value prevented")

test_uri_injection()
print("  [PASS] URI injection prevented")

test_method_injection()
print("  [PASS] Method injection prevented")

test_status_code_validation()
print("  [PASS] Invalid status codes rejected")

test_valid_status_codes()
print("  [PASS] Valid status codes accepted")

test_control_char_rejection()
print("  [PASS] Control characters rejected")

test_tab_in_header_allowed()
print("  [PASS] Tab in header value allowed")

test_empty_header_name_rejected()
print("  [PASS] Empty header name rejected")

test_header_count_limit()
print("  [PASS] Header count limit enforced")

test_special_chars_in_token()
print("  [PASS] Invalid token characters rejected")

print("all http security tests passed")

-- Explicit cleanup to avoid segfault on exit
collectgarbage("collect")
collectgarbage("collect")
