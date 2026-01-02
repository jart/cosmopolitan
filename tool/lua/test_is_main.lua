-- Tests for cosmo.is_main()
local cosmo = require("cosmo")
local unix = require("cosmo.unix")

-- Test 1: is_main() should exist and be a function
assert(type(cosmo.is_main) == "function", "is_main should be a function")

-- Test 2: When run directly, is_main() should return true
local result = cosmo.is_main()
assert(result == true, "is_main() should return true when script is run directly")

-- Test 3: When a module is require()'d, is_main() should return false for that module
local helper = require("test_is_main_helper")
assert(helper.is_main_when_required == false,
  "is_main() should return false when called during require()")

-- Test 4: Calling is_main() from a function in a required module should still
-- reflect the module's context (false), not the caller's context
assert(helper.check_is_main() == false,
  "is_main() called from required module function should return false")

-- Test 5: Test with subprocess to verify real-world main script detection
local tmpdir = os.getenv("TMPDIR") or "/tmp"
local test_script = tmpdir .. "/test_is_main_" .. tostring(os.time()) .. ".lua"

-- Write a test script
local f = io.open(test_script, "w")
f:write([[
local cosmo = require("cosmo")
if cosmo.is_main() then
  print("MAIN:true")
else
  print("MAIN:false")
end
]])
f:close()

-- Run the test script and capture output
local lua_path = arg[-1] or "lua"
local handle = io.popen(lua_path .. " " .. test_script .. " 2>&1")
local output = handle:read("*a")
handle:close()

-- Clean up
os.remove(test_script)

assert(output:match("MAIN:true"),
  "Subprocess running script directly should see is_main() == true, got: " .. output)

print("all is_main tests passed")
