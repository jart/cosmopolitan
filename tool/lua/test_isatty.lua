-- Test for unix.isatty
local unix = require("cosmo.unix")

-- Test 1: isatty on a regular file should return false
-- This demonstrates the semantic bug where it currently returns true
local fd = unix.open("/dev/null", unix.O_RDONLY)
assert(fd, "should be able to open /dev/null")

local result = unix.isatty(fd)
assert(result == false, "isatty on /dev/null should return false, got: " .. tostring(result))

unix.close(fd)

-- Test 2: isatty on stdout (if it's a tty) should return true
-- This triggers the warning: "syscall supposed to return 0 / -1 but got 1"
local result = unix.isatty(1)
-- We can't assert the result because it depends on whether stdout is a tty
-- but if it IS a tty, this will trigger the warning
if result then
  print("stdout is a tty (this should trigger warning in buggy version)")
end

-- Test 3: isatty on an invalid/closed fd might return false or error depending on the fd number
-- For a high fd number that doesn't exist, it may return false (not a tty)
-- Let's just verify it doesn't crash and returns a boolean or nil
local result, err = unix.isatty(999999)
assert(type(result) == "boolean" or result == nil, "isatty should return boolean or nil")

print("all isatty tests passed")
