local cosmo = require("cosmo")

-- Test Strftime function exists
assert(type(cosmo.Strftime) == "function", "Strftime should be a function")
assert(type(cosmo.Strptime) == "function", "Strptime should be a function")

-- Test Strftime with epoch (1970-01-01 00:00:00 UTC)
local s = cosmo.Strftime("%Y-%m-%d %H:%M:%S", 0)
assert(s == "1970-01-01 00:00:00", "epoch should format correctly, got: " .. tostring(s))

-- Test Strftime with a known timestamp
-- 1735500000 = 2024-12-29 18:40:00 UTC
local s = cosmo.Strftime("%Y-%m-%d", 1735500000)
assert(s == "2024-12-29", "date should format correctly, got: " .. tostring(s))

-- Test various format specifiers
local s = cosmo.Strftime("%F", 0)  -- %F = %Y-%m-%d
assert(s == "1970-01-01", "%F should work, got: " .. tostring(s))

local s = cosmo.Strftime("%T", 0)  -- %T = %H:%M:%S
assert(s == "00:00:00", "%T should work, got: " .. tostring(s))

-- Test weekday (epoch was Thursday)
local s = cosmo.Strftime("%a", 0)
assert(s == "Thu", "epoch weekday should be Thu, got: " .. tostring(s))

-- Test Strftime with default timestamp (current time)
local s = cosmo.Strftime("%Y")
assert(s ~= nil, "Strftime with default time should work")
assert(#s == 4, "year should be 4 digits")

-- Test Strptime basic parsing
local t = cosmo.Strptime("2024-12-29", "%Y-%m-%d")
assert(t ~= nil, "Strptime should return a table")
assert(t.year == 2024, "year should be 2024, got: " .. tostring(t.year))
assert(t.month == 12, "month should be 12, got: " .. tostring(t.month))
assert(t.day == 29, "day should be 29, got: " .. tostring(t.day))

-- Test Strptime with time
local t = cosmo.Strptime("2024-12-29 15:30:45", "%Y-%m-%d %H:%M:%S")
assert(t ~= nil, "Strptime with time should work")
assert(t.hour == 15, "hour should be 15, got: " .. tostring(t.hour))
assert(t.min == 30, "min should be 30, got: " .. tostring(t.min))
assert(t.sec == 45, "sec should be 45, got: " .. tostring(t.sec))

-- Test Strptime rest (unparsed remainder)
local t = cosmo.Strptime("2024-12-29 extra stuff", "%Y-%m-%d")
assert(t ~= nil, "Strptime with extra should work")
assert(t.rest == " extra stuff", "rest should contain unparsed part, got: " .. tostring(t.rest))

-- Test Strptime empty rest
local t = cosmo.Strptime("2024-12-29", "%Y-%m-%d")
assert(t.rest == "", "rest should be empty when fully parsed, got: " .. tostring(t.rest))

-- Test Strptime failure
local t, err = cosmo.Strptime("not-a-date", "%Y-%m-%d")
assert(t == nil, "Strptime should return nil on failure")
assert(err ~= nil, "Strptime should return error message on failure")

-- Test roundtrip: format then parse
local now = os.time()
local formatted = cosmo.Strftime("%Y-%m-%d %H:%M:%S", now)
local parsed = cosmo.Strptime(formatted, "%Y-%m-%d %H:%M:%S")
assert(parsed ~= nil, "roundtrip parse should succeed")
-- Note: can't compare exact time since Strftime uses UTC but os.time is local

print("PASS")
