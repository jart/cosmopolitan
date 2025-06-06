-- Test HTTPS connections through a proxy
-- Requires a proxy to be set in the environment variables

local function test_proxy_fetch()
  if os.getenv('http_proxy') or os.getenv('https_proxy') then
    local url = "http://www.google.com"
    print("Testing HTTP fetch through proxy: " .. url)

    local status, headers, _ = Fetch(url)

    if status == 200 then
      print("SUCCESS: Proxy connection worked")
      print("Status code: " .. status)
    else
      print("FAILED: Proxy connection failed")
      print("Reason: " .. headers)
    end

    url = "https://www.google.com"
    print("Testing HTTPS fetch through proxy: " .. url)

    local status2, headers2, _ = Fetch(url)

    if status2 == 200 then
      print("SUCCESS: Proxy connection worked")
      print("Status code: " .. status2)
    else
      print("FAILED: Proxy connection failed")
      print("Reason: " .. headers2)
    end
  else
    print("Skipping test: No proxy environment variables set (http_proxy or https_proxy)")
  end
end

test_proxy_fetch()
