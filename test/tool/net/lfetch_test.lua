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
function test_absolute_redirect()
    local status, headers, body = Fetch("https://httpbin.org/absolute-redirect/2")
    assert(status == 200, "expected 200 after absolute redirects, got: " .. tostring(status))
    print("test_absolute_redirect: PASS")
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
function test_custom_headers()
    local status, headers, body = Fetch("https://httpbin.org/headers", {
        headers = {
            ["X-Custom-Header"] = "test-value"
        }
    })
    assert(status == 200, "expected 200, got: " .. tostring(status))
    assert(body:find("X-Custom-Header") or body:find("x-custom-header"),
           "expected body to show custom header")
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
