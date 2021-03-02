-- $Id: testes/bitwise.lua $
-- See Copyright Notice in file all.lua

print("testing bitwise operations")

require "bwcoercion"

local numbits = string.packsize('j') * 8

assert(~0 == -1)

assert((1 << (numbits - 1)) == math.mininteger)

-- basic tests for bitwise operators;
-- use variables to avoid constant folding
local a, b, c, d
a = 0xFFFFFFFFFFFFFFFF
assert(a == -1 and a & -1 == a and a & 35 == 35)
a = 0xF0F0F0F0F0F0F0F0
assert(a | -1 == -1)
assert(a ~ a == 0 and a ~ 0 == a and a ~ ~a == -1)
assert(a >> 4 == ~a)
a = 0xF0; b = 0xCC; c = 0xAA; d = 0xFD
assert(a | b ~ c & d == 0xF4)

a = 0xF0.0; b = 0xCC.0; c = "0xAA.0"; d = "0xFD.0"
assert(a | b ~ c & d == 0xF4)

a = 0xF0000000; b = 0xCC000000;
c = 0xAA000000; d = 0xFD000000
assert(a | b ~ c & d == 0xF4000000)
assert(~~a == a and ~a == -1 ~ a and -d == ~d + 1)

a = a << 32
b = b << 32
c = c << 32
d = d << 32
assert(a | b ~ c & d == 0xF4000000 << 32)
assert(~~a == a and ~a == -1 ~ a and -d == ~d + 1)

assert(-1 >> 1 == (1 << (numbits - 1)) - 1 and 1 << 31 == 0x80000000)
assert(-1 >> (numbits - 1) == 1)
assert(-1 >> numbits == 0 and
       -1 >> -numbits == 0 and
       -1 << numbits == 0 and
       -1 << -numbits == 0)

assert((2^30 - 1) << 2^30 == 0)
assert((2^30 - 1) >> 2^30 == 0)

assert(1 >> -3 == 1 << 3 and 1000 >> 5 == 1000 << -5)


-- coercion from strings to integers
assert("0xffffffffffffffff" | 0 == -1)
assert("0xfffffffffffffffe" & "-1" == -2)
assert(" \t-0xfffffffffffffffe\n\t" & "-1" == 2)
assert("   \n  -45  \t " >> "  -2  " == -45 * 4)
assert("1234.0" << "5.0" == 1234 * 32)
assert("0xffff.0" ~ "0xAAAA" == 0x5555)
assert(~"0x0.000p4" == -1)

assert(("7" .. 3) << 1 == 146)
assert(0xffffffff >> (1 .. "9") == 0x1fff)
assert(10 | (1 .. "9") == 27)

do
  local st, msg = pcall(function () return 4 & "a" end)
  assert(string.find(msg, "'band'"))

  local st, msg = pcall(function () return ~"a" end)
  assert(string.find(msg, "'bnot'"))
end


-- out of range number
assert(not pcall(function () return "0xffffffffffffffff.0" | 0 end))

-- embedded zeros
assert(not pcall(function () return "0xffffffffffffffff\0" | 0 end))

print'+'


package.preload.bit32 = function ()     --{

-- no built-in 'bit32' library: implement it using bitwise operators

local bit = {}

function bit.bnot (a)
  return ~a & 0xFFFFFFFF
end


--
-- in all vararg functions, avoid creating 'arg' table when there are
-- only 2 (or less) parameters, as 2 parameters is the common case
--

function bit.band (x, y, z, ...)
  if not z then
    return ((x or -1) & (y or -1)) & 0xFFFFFFFF
  else
    local arg = {...}
    local res = x & y & z
    for i = 1, #arg do res = res & arg[i] end
    return res & 0xFFFFFFFF
  end
end

function bit.bor (x, y, z, ...)
  if not z then
    return ((x or 0) | (y or 0)) & 0xFFFFFFFF
  else
    local arg = {...}
    local res = x | y | z
    for i = 1, #arg do res = res | arg[i] end
    return res & 0xFFFFFFFF
  end
end

function bit.bxor (x, y, z, ...)
  if not z then
    return ((x or 0) ~ (y or 0)) & 0xFFFFFFFF
  else
    local arg = {...}
    local res = x ~ y ~ z
    for i = 1, #arg do res = res ~ arg[i] end
    return res & 0xFFFFFFFF
  end
end

function bit.btest (...)
  return bit.band(...) ~= 0
end

function bit.lshift (a, b)
  return ((a & 0xFFFFFFFF) << b) & 0xFFFFFFFF
end

function bit.rshift (a, b)
  return ((a & 0xFFFFFFFF) >> b) & 0xFFFFFFFF
end

function bit.arshift (a, b)
  a = a & 0xFFFFFFFF
  if b <= 0 or (a & 0x80000000) == 0 then
    return (a >> b) & 0xFFFFFFFF
  else
    return ((a >> b) | ~(0xFFFFFFFF >> b)) & 0xFFFFFFFF
  end
end

function bit.lrotate (a ,b)
  b = b & 31
  a = a & 0xFFFFFFFF
  a = (a << b) | (a >> (32 - b))
  return a & 0xFFFFFFFF
end

function bit.rrotate (a, b)
  return bit.lrotate(a, -b)
end

local function checkfield (f, w)
  w = w or 1
  assert(f >= 0, "field cannot be negative")
  assert(w > 0, "width must be positive")
  assert(f + w <= 32, "trying to access non-existent bits")
  return f, ~(-1 << w)
end

function bit.extract (a, f, w)
  local f, mask = checkfield(f, w)
  return (a >> f) & mask
end

function bit.replace (a, v, f, w)
  local f, mask = checkfield(f, w)
  v = v & mask
  a = (a & ~(mask << f)) | (v << f)
  return a & 0xFFFFFFFF
end

return bit

end  --}


print("testing bitwise library")

local bit32 = require'bit32'

assert(bit32.band() == bit32.bnot(0))
assert(bit32.btest() == true)
assert(bit32.bor() == 0)
assert(bit32.bxor() == 0)

assert(bit32.band() == bit32.band(0xffffffff))
assert(bit32.band(1,2) == 0)


-- out-of-range numbers
assert(bit32.band(-1) == 0xffffffff)
assert(bit32.band((1 << 33) - 1) == 0xffffffff)
assert(bit32.band(-(1 << 33) - 1) == 0xffffffff)
assert(bit32.band((1 << 33) + 1) == 1)
assert(bit32.band(-(1 << 33) + 1) == 1)
assert(bit32.band(-(1 << 40)) == 0)
assert(bit32.band(1 << 40) == 0)
assert(bit32.band(-(1 << 40) - 2) == 0xfffffffe)
assert(bit32.band((1 << 40) - 4) == 0xfffffffc)

assert(bit32.lrotate(0, -1) == 0)
assert(bit32.lrotate(0, 7) == 0)
assert(bit32.lrotate(0x12345678, 0) == 0x12345678)
assert(bit32.lrotate(0x12345678, 32) == 0x12345678)
assert(bit32.lrotate(0x12345678, 4) == 0x23456781)
assert(bit32.rrotate(0x12345678, -4) == 0x23456781)
assert(bit32.lrotate(0x12345678, -8) == 0x78123456)
assert(bit32.rrotate(0x12345678, 8) == 0x78123456)
assert(bit32.lrotate(0xaaaaaaaa, 2) == 0xaaaaaaaa)
assert(bit32.lrotate(0xaaaaaaaa, -2) == 0xaaaaaaaa)
for i = -50, 50 do
  assert(bit32.lrotate(0x89abcdef, i) == bit32.lrotate(0x89abcdef, i%32))
end

assert(bit32.lshift(0x12345678, 4) == 0x23456780)
assert(bit32.lshift(0x12345678, 8) == 0x34567800)
assert(bit32.lshift(0x12345678, -4) == 0x01234567)
assert(bit32.lshift(0x12345678, -8) == 0x00123456)
assert(bit32.lshift(0x12345678, 32) == 0)
assert(bit32.lshift(0x12345678, -32) == 0)
assert(bit32.rshift(0x12345678, 4) == 0x01234567)
assert(bit32.rshift(0x12345678, 8) == 0x00123456)
assert(bit32.rshift(0x12345678, 32) == 0)
assert(bit32.rshift(0x12345678, -32) == 0)
assert(bit32.arshift(0x12345678, 0) == 0x12345678)
assert(bit32.arshift(0x12345678, 1) == 0x12345678 // 2)
assert(bit32.arshift(0x12345678, -1) == 0x12345678 * 2)
assert(bit32.arshift(-1, 1) == 0xffffffff)
assert(bit32.arshift(-1, 24) == 0xffffffff)
assert(bit32.arshift(-1, 32) == 0xffffffff)
assert(bit32.arshift(-1, -1) == bit32.band(-1 * 2, 0xffffffff))

assert(0x12345678 << 4 == 0x123456780)
assert(0x12345678 << 8 == 0x1234567800)
assert(0x12345678 << -4 == 0x01234567)
assert(0x12345678 << -8 == 0x00123456)
assert(0x12345678 << 32 == 0x1234567800000000)
assert(0x12345678 << -32 == 0)
assert(0x12345678 >> 4 == 0x01234567)
assert(0x12345678 >> 8 == 0x00123456)
assert(0x12345678 >> 32 == 0)
assert(0x12345678 >> -32 == 0x1234567800000000)

print("+")
-- some special cases
local c = {0, 1, 2, 3, 10, 0x80000000, 0xaaaaaaaa, 0x55555555,
           0xffffffff, 0x7fffffff}

for _, b in pairs(c) do
  assert(bit32.band(b) == b)
  assert(bit32.band(b, b) == b)
  assert(bit32.band(b, b, b, b) == b)
  assert(bit32.btest(b, b) == (b ~= 0))
  assert(bit32.band(b, b, b) == b)
  assert(bit32.band(b, b, b, ~b) == 0)
  assert(bit32.btest(b, b, b) == (b ~= 0))
  assert(bit32.band(b, bit32.bnot(b)) == 0)
  assert(bit32.bor(b, bit32.bnot(b)) == bit32.bnot(0))
  assert(bit32.bor(b) == b)
  assert(bit32.bor(b, b) == b)
  assert(bit32.bor(b, b, b) == b)
  assert(bit32.bor(b, b, 0, ~b) == 0xffffffff)
  assert(bit32.bxor(b) == b)
  assert(bit32.bxor(b, b) == 0)
  assert(bit32.bxor(b, b, b) == b)
  assert(bit32.bxor(b, b, b, b) == 0)
  assert(bit32.bxor(b, 0) == b)
  assert(bit32.bnot(b) ~= b)
  assert(bit32.bnot(bit32.bnot(b)) == b)
  assert(bit32.bnot(b) == (1 << 32) - 1 - b)
  assert(bit32.lrotate(b, 32) == b)
  assert(bit32.rrotate(b, 32) == b)
  assert(bit32.lshift(bit32.lshift(b, -4), 4) == bit32.band(b, bit32.bnot(0xf)))
  assert(bit32.rshift(bit32.rshift(b, 4), -4) == bit32.band(b, bit32.bnot(0xf)))
end

-- for this test, use at most 24 bits (mantissa of a single float)
c = {0, 1, 2, 3, 10, 0x800000, 0xaaaaaa, 0x555555, 0xffffff, 0x7fffff}
for _, b in pairs(c) do
  for i = -40, 40 do
    local x = bit32.lshift(b, i)
    local y = math.floor(math.fmod(b * 2.0^i, 2.0^32))
    assert(math.fmod(x - y, 2.0^32) == 0)
  end
end

assert(not pcall(bit32.band, {}))
assert(not pcall(bit32.bnot, "a"))
assert(not pcall(bit32.lshift, 45))
assert(not pcall(bit32.lshift, 45, print))
assert(not pcall(bit32.rshift, 45, print))

print("+")


-- testing extract/replace

assert(bit32.extract(0x12345678, 0, 4) == 8)
assert(bit32.extract(0x12345678, 4, 4) == 7)
assert(bit32.extract(0xa0001111, 28, 4) == 0xa)
assert(bit32.extract(0xa0001111, 31, 1) == 1)
assert(bit32.extract(0x50000111, 31, 1) == 0)
assert(bit32.extract(0xf2345679, 0, 32) == 0xf2345679)

assert(not pcall(bit32.extract, 0, -1))
assert(not pcall(bit32.extract, 0, 32))
assert(not pcall(bit32.extract, 0, 0, 33))
assert(not pcall(bit32.extract, 0, 31, 2))

assert(bit32.replace(0x12345678, 5, 28, 4) == 0x52345678)
assert(bit32.replace(0x12345678, 0x87654321, 0, 32) == 0x87654321)
assert(bit32.replace(0, 1, 2) == 2^2)
assert(bit32.replace(0, -1, 4) == 2^4)
assert(bit32.replace(-1, 0, 31) == (1 << 31) - 1)
assert(bit32.replace(-1, 0, 1, 2) == (1 << 32) - 7)


-- testing conversion of floats

assert(bit32.bor(3.0) == 3)
assert(bit32.bor(-4.0) == 0xfffffffc)

-- large floats and large-enough integers?
if 2.0^50 < 2.0^50 + 1.0 and 2.0^50 < (-1 >> 1) then
  assert(bit32.bor(2.0^32 - 5.0) == 0xfffffffb)
  assert(bit32.bor(-2.0^32 - 6.0) == 0xfffffffa)
  assert(bit32.bor(2.0^48 - 5.0) == 0xfffffffb)
  assert(bit32.bor(-2.0^48 - 6.0) == 0xfffffffa)
end

print'OK'

