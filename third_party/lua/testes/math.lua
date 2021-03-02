-- $Id: testes/math.lua $
-- See Copyright Notice in file all.lua

print("testing numbers and math lib")

local minint <const> = math.mininteger
local maxint <const> = math.maxinteger

local intbits <const> = math.floor(math.log(maxint, 2) + 0.5) + 1
assert((1 << intbits) == 0)

assert(minint == 1 << (intbits - 1))
assert(maxint == minint - 1)

-- number of bits in the mantissa of a floating-point number
local floatbits = 24
do
  local p = 2.0^floatbits
  while p < p + 1.0 do
    p = p * 2.0
    floatbits = floatbits + 1
  end
end

local function isNaN (x)
  return (x ~= x)
end

assert(isNaN(0/0))
assert(not isNaN(1/0))


do
  local x = 2.0^floatbits
  assert(x > x - 1.0 and x == x + 1.0)

  print(string.format("%d-bit integers, %d-bit (mantissa) floats",
                       intbits, floatbits))
end

assert(math.type(0) == "integer" and math.type(0.0) == "float"
       and not math.type("10"))


local function checkerror (msg, f, ...)
  local s, err = pcall(f, ...)
  assert(not s and string.find(err, msg))
end

local msgf2i = "number.* has no integer representation"

-- float equality
function eq (a,b,limit)
  if not limit then
    if floatbits >= 50 then limit = 1E-11
    else limit = 1E-5
    end
  end
  -- a == b needed for +inf/-inf
  return a == b or math.abs(a-b) <= limit
end


-- equality with types
function eqT (a,b)
  return a == b and math.type(a) == math.type(b)
end


-- basic float notation
assert(0e12 == 0 and .0 == 0 and 0. == 0 and .2e2 == 20 and 2.E-1 == 0.2)

do
  local a,b,c = "2", " 3e0 ", " 10  "
  assert(a+b == 5 and -b == -3 and b+"2" == 5 and "10"-c == 0)
  assert(type(a) == 'string' and type(b) == 'string' and type(c) == 'string')
  assert(a == "2" and b == " 3e0 " and c == " 10  " and -c == -"  10 ")
  assert(c%a == 0 and a^b == 08)
  a = 0
  assert(a == -a and 0 == -0)
end

do
  local x = -1
  local mz = 0/x   -- minus zero
  t = {[0] = 10, 20, 30, 40, 50}
  assert(t[mz] == t[0] and t[-0] == t[0])
end

do   -- tests for 'modf'
  local a,b = math.modf(3.5)
  assert(a == 3.0 and b == 0.5)
  a,b = math.modf(-2.5)
  assert(a == -2.0 and b == -0.5)
  a,b = math.modf(-3e23)
  assert(a == -3e23 and b == 0.0)
  a,b = math.modf(3e35)
  assert(a == 3e35 and b == 0.0)
  a,b = math.modf(-1/0)   -- -inf
  assert(a == -1/0 and b == 0.0)
  a,b = math.modf(1/0)   -- inf
  assert(a == 1/0 and b == 0.0)
  a,b = math.modf(0/0)   -- NaN
  assert(isNaN(a) and isNaN(b))
  a,b = math.modf(3)  -- integer argument
  assert(eqT(a, 3) and eqT(b, 0.0))
  a,b = math.modf(minint)
  assert(eqT(a, minint) and eqT(b, 0.0))
end

assert(math.huge > 10e30)
assert(-math.huge < -10e30)


-- integer arithmetic
assert(minint < minint + 1)
assert(maxint - 1 < maxint)
assert(0 - minint == minint)
assert(minint * minint == 0)
assert(maxint * maxint * maxint == maxint)


-- testing floor division and conversions

for _, i in pairs{-16, -15, -3, -2, -1, 0, 1, 2, 3, 15} do
  for _, j in pairs{-16, -15, -3, -2, -1, 1, 2, 3, 15} do
    for _, ti in pairs{0, 0.0} do     -- try 'i' as integer and as float
      for _, tj in pairs{0, 0.0} do   -- try 'j' as integer and as float
        local x = i + ti
        local y = j + tj
          assert(i//j == math.floor(i/j))
      end
    end
  end
end

assert(1//0.0 == 1/0)
assert(-1 // 0.0 == -1/0)
assert(eqT(3.5 // 1.5, 2.0))
assert(eqT(3.5 // -1.5, -3.0))

do   -- tests for different kinds of opcodes
  local x, y 
  x = 1; assert(x // 0.0 == 1/0)
  x = 1.0; assert(x // 0 == 1/0)
  x = 3.5; assert(eqT(x // 1, 3.0))
  assert(eqT(x // -1, -4.0))

  x = 3.5; y = 1.5; assert(eqT(x // y, 2.0))
  x = 3.5; y = -1.5; assert(eqT(x // y, -3.0))
end

assert(maxint // maxint == 1)
assert(maxint // 1 == maxint)
assert((maxint - 1) // maxint == 0)
assert(maxint // (maxint - 1) == 1)
assert(minint // minint == 1)
assert(minint // minint == 1)
assert((minint + 1) // minint == 0)
assert(minint // (minint + 1) == 1)
assert(minint // 1 == minint)

assert(minint // -1 == -minint)
assert(minint // -2 == 2^(intbits - 2))
assert(maxint // -1 == -maxint)


-- negative exponents
do
  assert(2^-3 == 1 / 2^3)
  assert(eq((-3)^-3, 1 / (-3)^3))
  for i = -3, 3 do    -- variables avoid constant folding
      for j = -3, 3 do
        -- domain errors (0^(-n)) are not portable
        if not _port or i ~= 0 or j > 0 then
          assert(eq(i^j, 1 / i^(-j)))
       end
    end
  end
end

-- comparison between floats and integers (border cases)
if floatbits < intbits then
  assert(2.0^floatbits == (1 << floatbits))
  assert(2.0^floatbits - 1.0 == (1 << floatbits) - 1.0)
  assert(2.0^floatbits - 1.0 ~= (1 << floatbits))
  -- float is rounded, int is not
  assert(2.0^floatbits + 1.0 ~= (1 << floatbits) + 1)
else   -- floats can express all integers with full accuracy
  assert(maxint == maxint + 0.0)
  assert(maxint - 1 == maxint - 1.0)
  assert(minint + 1 == minint + 1.0)
  assert(maxint ~= maxint - 1.0)
end
assert(maxint + 0.0 == 2.0^(intbits - 1) - 1.0)
assert(minint + 0.0 == minint)
assert(minint + 0.0 == -2.0^(intbits - 1))


-- order between floats and integers
assert(1 < 1.1); assert(not (1 < 0.9))
assert(1 <= 1.1); assert(not (1 <= 0.9))
assert(-1 < -0.9); assert(not (-1 < -1.1))
assert(1 <= 1.1); assert(not (-1 <= -1.1))
assert(-1 < -0.9); assert(not (-1 < -1.1))
assert(-1 <= -0.9); assert(not (-1 <= -1.1))
assert(minint <= minint + 0.0)
assert(minint + 0.0 <= minint)
assert(not (minint < minint + 0.0))
assert(not (minint + 0.0 < minint))
assert(maxint < minint * -1.0)
assert(maxint <= minint * -1.0)

do
  local fmaxi1 = 2^(intbits - 1)
  assert(maxint < fmaxi1)
  assert(maxint <= fmaxi1)
  assert(not (fmaxi1 <= maxint))
  assert(minint <= -2^(intbits - 1))
  assert(-2^(intbits - 1) <= minint)
end

if floatbits < intbits then
  print("testing order (floats cannot represent all integers)")
  local fmax = 2^floatbits
  local ifmax = fmax | 0
  assert(fmax < ifmax + 1)
  assert(fmax - 1 < ifmax)
  assert(-(fmax - 1) > -ifmax)
  assert(not (fmax <= ifmax - 1))
  assert(-fmax > -(ifmax + 1))
  assert(not (-fmax >= -(ifmax - 1)))

  assert(fmax/2 - 0.5 < ifmax//2)
  assert(-(fmax/2 - 0.5) > -ifmax//2)

  assert(maxint < 2^intbits)
  assert(minint > -2^intbits)
  assert(maxint <= 2^intbits)
  assert(minint >= -2^intbits)
else
  print("testing order (floats can represent all integers)")
  assert(maxint < maxint + 1.0)
  assert(maxint < maxint + 0.5)
  assert(maxint - 1.0 < maxint)
  assert(maxint - 0.5 < maxint)
  assert(not (maxint + 0.0 < maxint))
  assert(maxint + 0.0 <= maxint)
  assert(not (maxint < maxint + 0.0))
  assert(maxint + 0.0 <= maxint)
  assert(maxint <= maxint + 0.0)
  assert(not (maxint + 1.0 <= maxint))
  assert(not (maxint + 0.5 <= maxint))
  assert(not (maxint <= maxint - 1.0))
  assert(not (maxint <= maxint - 0.5))

  assert(minint < minint + 1.0)
  assert(minint < minint + 0.5)
  assert(minint <= minint + 0.5)
  assert(minint - 1.0 < minint)
  assert(minint - 1.0 <= minint)
  assert(not (minint + 0.0 < minint))
  assert(not (minint + 0.5 < minint))
  assert(not (minint < minint + 0.0))
  assert(minint + 0.0 <= minint)
  assert(minint <= minint + 0.0)
  assert(not (minint + 1.0 <= minint))
  assert(not (minint + 0.5 <= minint))
  assert(not (minint <= minint - 1.0))
end

do
  local NaN <const> = 0/0
  assert(not (NaN < 0))
  assert(not (NaN > minint))
  assert(not (NaN <= -9))
  assert(not (NaN <= maxint))
  assert(not (NaN < maxint))
  assert(not (minint <= NaN))
  assert(not (minint < NaN))
  assert(not (4 <= NaN))
  assert(not (4 < NaN))
end


-- avoiding errors at compile time
local function checkcompt (msg, code)
  checkerror(msg, assert(load(code)))
end
checkcompt("divide by zero", "return 2 // 0")
checkcompt(msgf2i, "return 2.3 >> 0")
checkcompt(msgf2i, ("return 2.0^%d & 1"):format(intbits - 1))
checkcompt("field 'huge'", "return math.huge << 1")
checkcompt(msgf2i, ("return 1 | 2.0^%d"):format(intbits - 1))
checkcompt(msgf2i, "return 2.3 ~ 0.0")


-- testing overflow errors when converting from float to integer (runtime)
local function f2i (x) return x | x end
checkerror(msgf2i, f2i, math.huge)     -- +inf
checkerror(msgf2i, f2i, -math.huge)    -- -inf
checkerror(msgf2i, f2i, 0/0)           -- NaN

if floatbits < intbits then
  -- conversion tests when float cannot represent all integers
  assert(maxint + 1.0 == maxint + 0.0)
  assert(minint - 1.0 == minint + 0.0)
  checkerror(msgf2i, f2i, maxint + 0.0)
  assert(f2i(2.0^(intbits - 2)) == 1 << (intbits - 2))
  assert(f2i(-2.0^(intbits - 2)) == -(1 << (intbits - 2)))
  assert((2.0^(floatbits - 1) + 1.0) // 1 == (1 << (floatbits - 1)) + 1)
  -- maximum integer representable as a float
  local mf = maxint - (1 << (floatbits - intbits)) + 1
  assert(f2i(mf + 0.0) == mf)  -- OK up to here
  mf = mf + 1
  assert(f2i(mf + 0.0) ~= mf)   -- no more representable
else
  -- conversion tests when float can represent all integers
  assert(maxint + 1.0 > maxint)
  assert(minint - 1.0 < minint)
  assert(f2i(maxint + 0.0) == maxint)
  checkerror("no integer rep", f2i, maxint + 1.0)
  checkerror("no integer rep", f2i, minint - 1.0)
end

-- 'minint' should be representable as a float no matter the precision
assert(f2i(minint + 0.0) == minint)


-- testing numeric strings

assert("2" + 1 == 3)
assert("2 " + 1 == 3)
assert(" -2 " + 1 == -1)
assert(" -0xa " + 1 == -9)


-- Literal integer Overflows (new behavior in 5.3.3)
do
  -- no overflows
  assert(eqT(tonumber(tostring(maxint)), maxint))
  assert(eqT(tonumber(tostring(minint)), minint))

  -- add 1 to last digit as a string (it cannot be 9...)
  local function incd (n)
    local s = string.format("%d", n)
    s = string.gsub(s, "%d$", function (d)
          assert(d ~= '9')
          return string.char(string.byte(d) + 1)
        end)
    return s
  end

  -- 'tonumber' with overflow by 1
  assert(eqT(tonumber(incd(maxint)), maxint + 1.0))
  assert(eqT(tonumber(incd(minint)), minint - 1.0))

  -- large numbers
  assert(eqT(tonumber("1"..string.rep("0", 30)), 1e30))
  assert(eqT(tonumber("-1"..string.rep("0", 30)), -1e30))

  -- hexa format still wraps around
  assert(eqT(tonumber("0x1"..string.rep("0", 30)), 0))

  -- lexer in the limits
  assert(minint == load("return " .. minint)())
  assert(eqT(maxint, load("return " .. maxint)()))

  assert(eqT(10000000000000000000000.0, 10000000000000000000000))
  assert(eqT(-10000000000000000000000.0, -10000000000000000000000))
end


-- testing 'tonumber'

-- 'tonumber' with numbers
assert(tonumber(3.4) == 3.4)
assert(eqT(tonumber(3), 3))
assert(eqT(tonumber(maxint), maxint) and eqT(tonumber(minint), minint))
assert(tonumber(1/0) == 1/0)

-- 'tonumber' with strings
assert(tonumber("0") == 0)
assert(not tonumber(""))
assert(not tonumber("  "))
assert(not tonumber("-"))
assert(not tonumber("  -0x "))
assert(not tonumber{})
assert(tonumber'+0.01' == 1/100 and tonumber'+.01' == 0.01 and
       tonumber'.01' == 0.01    and tonumber'-1.' == -1 and
       tonumber'+1.' == 1)
assert(not tonumber'+ 0.01' and not tonumber'+.e1' and
       not tonumber'1e'     and not tonumber'1.0e+' and
       not tonumber'.')
assert(tonumber('-012') == -010-2)
assert(tonumber('-1.2e2') == - - -120)

assert(tonumber("0xffffffffffff") == (1 << (4*12)) - 1)
assert(tonumber("0x"..string.rep("f", (intbits//4))) == -1)
assert(tonumber("-0x"..string.rep("f", (intbits//4))) == 1)

-- testing 'tonumber' with base
assert(tonumber('  001010  ', 2) == 10)
assert(tonumber('  001010  ', 10) == 001010)
assert(tonumber('  -1010  ', 2) == -10)
assert(tonumber('10', 36) == 36)
assert(tonumber('  -10  ', 36) == -36)
assert(tonumber('  +1Z  ', 36) == 36 + 35)
assert(tonumber('  -1z  ', 36) == -36 + -35)
assert(tonumber('-fFfa', 16) == -(10+(16*(15+(16*(15+(16*15)))))))
assert(tonumber(string.rep('1', (intbits - 2)), 2) + 1 == 2^(intbits - 2))
assert(tonumber('ffffFFFF', 16)+1 == (1 << 32))
assert(tonumber('0ffffFFFF', 16)+1 == (1 << 32))
assert(tonumber('-0ffffffFFFF', 16) - 1 == -(1 << 40))
for i = 2,36 do
  local i2 = i * i
  local i10 = i2 * i2 * i2 * i2 * i2      -- i^10
  assert(tonumber('\t10000000000\t', i) == i10)
end

if not _soft then
  -- tests with very long numerals
  assert(tonumber("0x"..string.rep("f", 13)..".0") == 2.0^(4*13) - 1)
  assert(tonumber("0x"..string.rep("f", 150)..".0") == 2.0^(4*150) - 1)
  assert(tonumber("0x"..string.rep("f", 300)..".0") == 2.0^(4*300) - 1)
  assert(tonumber("0x"..string.rep("f", 500)..".0") == 2.0^(4*500) - 1)
  assert(tonumber('0x3.' .. string.rep('0', 1000)) == 3)
  assert(tonumber('0x' .. string.rep('0', 1000) .. 'a') == 10)
  assert(tonumber('0x0.' .. string.rep('0', 13).."1") == 2.0^(-4*14))
  assert(tonumber('0x0.' .. string.rep('0', 150).."1") == 2.0^(-4*151))
  assert(tonumber('0x0.' .. string.rep('0', 300).."1") == 2.0^(-4*301))
  assert(tonumber('0x0.' .. string.rep('0', 500).."1") == 2.0^(-4*501))

  assert(tonumber('0xe03' .. string.rep('0', 1000) .. 'p-4000') == 3587.0)
  assert(tonumber('0x.' .. string.rep('0', 1000) .. '74p4004') == 0x7.4)
end

-- testing 'tonumber' for invalid formats

local function f (...)
  if select('#', ...) == 1 then
    return (...)
  else
    return "***"
  end
end

assert(not f(tonumber('fFfa', 15)))
assert(not f(tonumber('099', 8)))
assert(not f(tonumber('1\0', 2)))
assert(not f(tonumber('', 8)))
assert(not f(tonumber('  ', 9)))
assert(not f(tonumber('  ', 9)))
assert(not f(tonumber('0xf', 10)))

assert(not f(tonumber('inf')))
assert(not f(tonumber(' INF ')))
assert(not f(tonumber('Nan')))
assert(not f(tonumber('nan')))

assert(not f(tonumber('  ')))
assert(not f(tonumber('')))
assert(not f(tonumber('1  a')))
assert(not f(tonumber('1  a', 2)))
assert(not f(tonumber('1\0')))
assert(not f(tonumber('1 \0')))
assert(not f(tonumber('1\0 ')))
assert(not f(tonumber('e1')))
assert(not f(tonumber('e  1')))
assert(not f(tonumber(' 3.4.5 ')))


-- testing 'tonumber' for invalid hexadecimal formats

assert(not tonumber('0x'))
assert(not tonumber('x'))
assert(not tonumber('x3'))
assert(not tonumber('0x3.3.3'))   -- two decimal points
assert(not tonumber('00x2'))
assert(not tonumber('0x 2'))
assert(not tonumber('0 x2'))
assert(not tonumber('23x'))
assert(not tonumber('- 0xaa'))
assert(not tonumber('-0xaaP '))   -- no exponent
assert(not tonumber('0x0.51p'))
assert(not tonumber('0x5p+-2'))


-- testing hexadecimal numerals

assert(0x10 == 16 and 0xfff == 2^12 - 1 and 0XFB == 251)
assert(0x0p12 == 0 and 0x.0p-3 == 0)
assert(0xFFFFFFFF == (1 << 32) - 1)
assert(tonumber('+0x2') == 2)
assert(tonumber('-0xaA') == -170)
assert(tonumber('-0xffFFFfff') == -(1 << 32) + 1)

-- possible confusion with decimal exponent
assert(0E+1 == 0 and 0xE+1 == 15 and 0xe-1 == 13)


-- floating hexas

assert(tonumber('  0x2.5  ') == 0x25/16)
assert(tonumber('  -0x2.5  ') == -0x25/16)
assert(tonumber('  +0x0.51p+8  ') == 0x51)
assert(0x.FfffFFFF == 1 - '0x.00000001')
assert('0xA.a' + 0 == 10 + 10/16)
assert(0xa.aP4 == 0XAA)
assert(0x4P-2 == 1)
assert(0x1.1 == '0x1.' + '+0x.1')
assert(0Xabcdef.0 == 0x.ABCDEFp+24)


assert(1.1 == 1.+.1)
assert(100.0 == 1E2 and .01 == 1e-2)
assert(1111111111 - 1111111110 == 1000.00e-03)
assert(1.1 == '1.'+'.1')
assert(tonumber'1111111111' - tonumber'1111111110' ==
       tonumber"  +0.001e+3 \n\t")

assert(0.1e-30 > 0.9E-31 and 0.9E30 < 0.1e31)

assert(0.123456 > 0.123455)

assert(tonumber('+1.23E18') == 1.23*10.0^18)

-- testing order operators
assert(not(1<1) and (1<2) and not(2<1))
assert(not('a'<'a') and ('a'<'b') and not('b'<'a'))
assert((1<=1) and (1<=2) and not(2<=1))
assert(('a'<='a') and ('a'<='b') and not('b'<='a'))
assert(not(1>1) and not(1>2) and (2>1))
assert(not('a'>'a') and not('a'>'b') and ('b'>'a'))
assert((1>=1) and not(1>=2) and (2>=1))
assert(('a'>='a') and not('a'>='b') and ('b'>='a'))
assert(1.3 < 1.4 and 1.3 <= 1.4 and not (1.3 < 1.3) and 1.3 <= 1.3)

-- testing mod operator
assert(eqT(-4 % 3, 2))
assert(eqT(4 % -3, -2))
assert(eqT(-4.0 % 3, 2.0))
assert(eqT(4 % -3.0, -2.0))
assert(eqT(4 % -5, -1))
assert(eqT(4 % -5.0, -1.0))
assert(eqT(4 % 5, 4))
assert(eqT(4 % 5.0, 4.0))
assert(eqT(-4 % -5, -4))
assert(eqT(-4 % -5.0, -4.0))
assert(eqT(-4 % 5, 1))
assert(eqT(-4 % 5.0, 1.0))
assert(eqT(4.25 % 4, 0.25))
assert(eqT(10.0 % 2, 0.0))
assert(eqT(-10.0 % 2, 0.0))
assert(eqT(-10.0 % -2, 0.0))
assert(math.pi - math.pi % 1 == 3)
assert(math.pi - math.pi % 0.001 == 3.141)

do   -- very small numbers
  local i, j = 0, 20000
  while i < j do
    local m = (i + j) // 2
    if 10^-m > 0 then
      i = m + 1
    else
      j = m
    end
  end
  -- 'i' is the smallest possible ten-exponent
  local b = 10^-(i - (i // 10))   -- a very small number
  assert(b > 0 and b * b == 0)
  local delta = b / 1000
  assert(eq((2.1 * b) % (2 * b), (0.1 * b), delta))
  assert(eq((-2.1 * b) % (2 * b), (2 * b) - (0.1 * b), delta))
  assert(eq((2.1 * b) % (-2 * b), (0.1 * b) - (2 * b), delta))
  assert(eq((-2.1 * b) % (-2 * b), (-0.1 * b), delta))
end


-- basic consistency between integer modulo and float modulo
for i = -10, 10 do
  for j = -10, 10 do
    if j ~= 0 then
      assert((i + 0.0) % j == i % j)
    end
  end
end

for i = 0, 10 do
  for j = -10, 10 do
    if j ~= 0 then
      assert((2^i) % j == (1 << i) % j)
    end
  end
end

do    -- precision of module for large numbers
  local i = 10
  while (1 << i) > 0 do
    assert((1 << i) % 3 == i % 2 + 1)
    i = i + 1
  end

  i = 10
  while 2^i < math.huge do
    assert(2^i % 3 == i % 2 + 1)
    i = i + 1
  end
end

assert(eqT(minint % minint, 0))
assert(eqT(maxint % maxint, 0))
assert((minint + 1) % minint == minint + 1)
assert((maxint - 1) % maxint == maxint - 1)
assert(minint % maxint == maxint - 1)

assert(minint % -1 == 0)
assert(minint % -2 == 0)
assert(maxint % -2 == -1)

-- non-portable tests because Windows C library cannot compute 
-- fmod(1, huge) correctly
if not _port then
  local function anan (x) assert(isNaN(x)) end   -- assert Not a Number
  anan(0.0 % 0)
  anan(1.3 % 0)
  anan(math.huge % 1)
  anan(math.huge % 1e30)
  anan(-math.huge % 1e30)
  anan(-math.huge % -1e30)
  assert(1 % math.huge == 1)
  assert(1e30 % math.huge == 1e30)
  assert(1e30 % -math.huge == -math.huge)
  assert(-1 % math.huge == math.huge)
  assert(-1 % -math.huge == -1)
end


-- testing unsigned comparisons
assert(math.ult(3, 4))
assert(not math.ult(4, 4))
assert(math.ult(-2, -1))
assert(math.ult(2, -1))
assert(not math.ult(-2, -2))
assert(math.ult(maxint, minint))
assert(not math.ult(minint, maxint))


assert(eq(math.sin(-9.8)^2 + math.cos(-9.8)^2, 1))
assert(eq(math.tan(math.pi/4), 1))
assert(eq(math.sin(math.pi/2), 1) and eq(math.cos(math.pi/2), 0))
assert(eq(math.atan(1), math.pi/4) and eq(math.acos(0), math.pi/2) and
       eq(math.asin(1), math.pi/2))
assert(eq(math.deg(math.pi/2), 90) and eq(math.rad(90), math.pi/2))
assert(math.abs(-10.43) == 10.43)
assert(eqT(math.abs(minint), minint))
assert(eqT(math.abs(maxint), maxint))
assert(eqT(math.abs(-maxint), maxint))
assert(eq(math.atan(1,0), math.pi/2))
assert(math.fmod(10,3) == 1)
assert(eq(math.sqrt(10)^2, 10))
assert(eq(math.log(2, 10), math.log(2)/math.log(10)))
assert(eq(math.log(2, 2), 1))
assert(eq(math.log(9, 3), 2))
assert(eq(math.exp(0), 1))
assert(eq(math.sin(10), math.sin(10%(2*math.pi))))


assert(tonumber(' 1.3e-2 ') == 1.3e-2)
assert(tonumber(' -1.00000000000001 ') == -1.00000000000001)

-- testing constant limits
-- 2^23 = 8388608
assert(8388609 + -8388609 == 0)
assert(8388608 + -8388608 == 0)
assert(8388607 + -8388607 == 0)



do   -- testing floor & ceil
  assert(eqT(math.floor(3.4), 3))
  assert(eqT(math.ceil(3.4), 4))
  assert(eqT(math.floor(-3.4), -4))
  assert(eqT(math.ceil(-3.4), -3))
  assert(eqT(math.floor(maxint), maxint))
  assert(eqT(math.ceil(maxint), maxint))
  assert(eqT(math.floor(minint), minint))
  assert(eqT(math.floor(minint + 0.0), minint))
  assert(eqT(math.ceil(minint), minint))
  assert(eqT(math.ceil(minint + 0.0), minint))
  assert(math.floor(1e50) == 1e50)
  assert(math.ceil(1e50) == 1e50)
  assert(math.floor(-1e50) == -1e50)
  assert(math.ceil(-1e50) == -1e50)
  for _, p in pairs{31,32,63,64} do
    assert(math.floor(2^p) == 2^p)
    assert(math.floor(2^p + 0.5) == 2^p)
    assert(math.ceil(2^p) == 2^p)
    assert(math.ceil(2^p - 0.5) == 2^p)
  end
  checkerror("number expected", math.floor, {})
  checkerror("number expected", math.ceil, print)
  assert(eqT(math.tointeger(minint), minint))
  assert(eqT(math.tointeger(minint .. ""), minint))
  assert(eqT(math.tointeger(maxint), maxint))
  assert(eqT(math.tointeger(maxint .. ""), maxint))
  assert(eqT(math.tointeger(minint + 0.0), minint))
  assert(not math.tointeger(0.0 - minint))
  assert(not math.tointeger(math.pi))
  assert(not math.tointeger(-math.pi))
  assert(math.floor(math.huge) == math.huge)
  assert(math.ceil(math.huge) == math.huge)
  assert(not math.tointeger(math.huge))
  assert(math.floor(-math.huge) == -math.huge)
  assert(math.ceil(-math.huge) == -math.huge)
  assert(not math.tointeger(-math.huge))
  assert(math.tointeger("34.0") == 34)
  assert(not math.tointeger("34.3"))
  assert(not math.tointeger({}))
  assert(not math.tointeger(0/0))    -- NaN
end


-- testing fmod for integers
for i = -6, 6 do
  for j = -6, 6 do
    if j ~= 0 then
      local mi = math.fmod(i, j)
      local mf = math.fmod(i + 0.0, j)
      assert(mi == mf)
      assert(math.type(mi) == 'integer' and math.type(mf) == 'float')
      if (i >= 0 and j >= 0) or (i <= 0 and j <= 0) or mi == 0 then
        assert(eqT(mi, i % j))
      end
    end
  end
end
assert(eqT(math.fmod(minint, minint), 0))
assert(eqT(math.fmod(maxint, maxint), 0))
assert(eqT(math.fmod(minint + 1, minint), minint + 1))
assert(eqT(math.fmod(maxint - 1, maxint), maxint - 1))

checkerror("zero", math.fmod, 3, 0)


do    -- testing max/min
  checkerror("value expected", math.max)
  checkerror("value expected", math.min)
  assert(eqT(math.max(3), 3))
  assert(eqT(math.max(3, 5, 9, 1), 9))
  assert(math.max(maxint, 10e60) == 10e60)
  assert(eqT(math.max(minint, minint + 1), minint + 1))
  assert(eqT(math.min(3), 3))
  assert(eqT(math.min(3, 5, 9, 1), 1))
  assert(math.min(3.2, 5.9, -9.2, 1.1) == -9.2)
  assert(math.min(1.9, 1.7, 1.72) == 1.7)
  assert(math.min(-10e60, minint) == -10e60)
  assert(eqT(math.min(maxint, maxint - 1), maxint - 1))
  assert(eqT(math.min(maxint - 2, maxint, maxint - 1), maxint - 2))
end
-- testing implicit conversions

local a,b = '10', '20'
assert(a*b == 200 and a+b == 30 and a-b == -10 and a/b == 0.5 and -b == -20)
assert(a == '10' and b == '20')


do
  print("testing -0 and NaN")
  local mz <const> = -0.0
  local z <const> = 0.0
  assert(mz == z)
  assert(1/mz < 0 and 0 < 1/z)
  local a = {[mz] = 1}
  assert(a[z] == 1 and a[mz] == 1)
  a[z] = 2
  assert(a[z] == 2 and a[mz] == 2)
  local inf = math.huge * 2 + 1
  local mz <const> = -1/inf
  local z <const> = 1/inf
  assert(mz == z)
  assert(1/mz < 0 and 0 < 1/z)
  local NaN <const> = inf - inf
  assert(NaN ~= NaN)
  assert(not (NaN < NaN))
  assert(not (NaN <= NaN))
  assert(not (NaN > NaN))
  assert(not (NaN >= NaN))
  assert(not (0 < NaN) and not (NaN < 0))
  local NaN1 <const> = 0/0
  assert(NaN ~= NaN1 and not (NaN <= NaN1) and not (NaN1 <= NaN))
  local a = {}
  assert(not pcall(rawset, a, NaN, 1))
  assert(a[NaN] == undef)
  a[1] = 1
  assert(not pcall(rawset, a, NaN, 1))
  assert(a[NaN] == undef)
  -- strings with same binary representation as 0.0 (might create problems
  -- for constant manipulation in the pre-compiler)
  local a1, a2, a3, a4, a5 = 0, 0, "\0\0\0\0\0\0\0\0", 0, "\0\0\0\0\0\0\0\0"
  assert(a1 == a2 and a2 == a4 and a1 ~= a3)
  assert(a3 == a5)
end


print("testing 'math.random'")

local random, max, min = math.random, math.max, math.min

local function testnear (val, ref, tol)
  return (math.abs(val - ref) < ref * tol)
end


-- low-level!! For the current implementation of random in Lua,
-- the first call after seed 1007 should return 0x7a7040a5a323c9d6
do
  -- all computations should work with 32-bit integers
  local h <const> = 0x7a7040a5   -- higher half
  local l <const> = 0xa323c9d6   -- lower half

  math.randomseed(1007)
  -- get the low 'intbits' of the 64-bit expected result
  local res = (h << 32 | l) & ~(~0 << intbits)
  assert(random(0) == res)

  math.randomseed(1007, 0)
  -- using higher bits to generate random floats; (the '% 2^32' converts
  -- 32-bit integers to floats as unsigned)
  local res
  if floatbits <= 32 then
    -- get all bits from the higher half
    res = (h >> (32 - floatbits)) % 2^32
  else
    -- get 32 bits from the higher half and the rest from the lower half
    res = (h % 2^32) * 2^(floatbits - 32) + ((l >> (64 - floatbits)) % 2^32)
  end
  local rand = random()
  assert(eq(rand, 0x0.7a7040a5a323c9d6, 2^-floatbits))
  assert(rand * 2^floatbits == res)
end

do
  -- testing return of 'randomseed'
  local x, y = math.randomseed()
  local res = math.random(0)
  x, y = math.randomseed(x, y)    -- should repeat the state
  assert(math.random(0) == res)
  math.randomseed(x, y)    -- again should repeat the state
  assert(math.random(0) == res)
  -- keep the random seed for following tests
end

do   -- test random for floats
  local randbits = math.min(floatbits, 64)   -- at most 64 random bits
  local mult = 2^randbits      -- to make random float into an integral
  local counts = {}    -- counts for bits
  for i = 1, randbits do counts[i] = 0 end
  local up = -math.huge
  local low = math.huge
  local rounds = 100 * randbits   -- 100 times for each bit
  local totalrounds = 0
  ::doagain::   -- will repeat test until we get good statistics
  for i = 0, rounds do
    local t = random()
    assert(0 <= t and t < 1)
    up = max(up, t)
    low = min(low, t)
    assert(t * mult % 1 == 0)    -- no extra bits
    local bit = i % randbits     -- bit to be tested
    if (t * 2^bit) % 1 >= 0.5 then    -- is bit set?
      counts[bit + 1] = counts[bit + 1] + 1   -- increment its count
    end
  end
  totalrounds = totalrounds + rounds
  if not (eq(up, 1, 0.001) and eq(low, 0, 0.001)) then
    goto doagain
  end
  -- all bit counts should be near 50%
  local expected = (totalrounds / randbits / 2)
  for i = 1, randbits do
    if not testnear(counts[i], expected, 0.10) then
      goto doagain
    end
  end
  print(string.format("float random range in %d calls: [%f, %f]",
                      totalrounds, low, up))
end


do   -- test random for full integers
  local up = 0
  local low = 0
  local counts = {}    -- counts for bits
  for i = 1, intbits do counts[i] = 0 end
  local rounds = 100 * intbits   -- 100 times for each bit
  local totalrounds = 0
  ::doagain::   -- will repeat test until we get good statistics
  for i = 0, rounds do
    local t = random(0)
    up = max(up, t)
    low = min(low, t)
    local bit = i % intbits     -- bit to be tested
    -- increment its count if it is set
    counts[bit + 1] = counts[bit + 1] + ((t >> bit) & 1)
  end
  totalrounds = totalrounds + rounds
  local lim = maxint >> 10
  if not (maxint - up < lim and low - minint < lim) then
    goto doagain
  end
  -- all bit counts should be near 50%
  local expected = (totalrounds / intbits / 2)
  for i = 1, intbits do
    if not testnear(counts[i], expected, 0.10) then
      goto doagain
    end
  end
  print(string.format(
     "integer random range in %d calls: [minint + %.0fppm, maxint - %.0fppm]",
      totalrounds, (minint - low) / minint * 1e6,
                   (maxint - up) / maxint * 1e6))
end

do
  -- test distribution for a dice
  local count = {0, 0, 0, 0, 0, 0}
  local rep = 200
  local totalrep = 0
  ::doagain::
  for i = 1, rep * 6 do
    local r = random(6)
    count[r] = count[r] + 1
  end
  totalrep = totalrep + rep
  for i = 1, 6 do
    if not testnear(count[i], totalrep, 0.05) then
      goto doagain
    end
  end
end

do
  local function aux (x1, x2)     -- test random for small intervals
    local mark = {}; local count = 0   -- to check that all values appeared
    while true do
      local t = random(x1, x2)
      assert(x1 <= t and t <= x2)
      if not mark[t] then  -- new value
        mark[t] = true
        count = count + 1
        if count == x2 - x1 + 1 then   -- all values appeared; OK
          goto ok
        end
      end
    end
   ::ok::
  end

  aux(-10,0)
  aux(1, 6)
  aux(1, 2)
  aux(1, 13)
  aux(1, 31)
  aux(1, 32)
  aux(1, 33)
  aux(-10, 10)
  aux(-10,-10)   -- unit set
  aux(minint, minint)   -- unit set
  aux(maxint, maxint)   -- unit set
  aux(minint, minint + 9)
  aux(maxint - 3, maxint)
end

do
  local function aux(p1, p2)       -- test random for large intervals
    local max = minint
    local min = maxint
    local n = 100
    local mark = {}; local count = 0   -- to count how many different values
    ::doagain::
    for _ = 1, n do
      local t = random(p1, p2)
      if not mark[t] then  -- new value
        assert(p1 <= t and t <= p2)
        max = math.max(max, t)
        min = math.min(min, t)
        mark[t] = true
        count = count + 1
      end
    end
    -- at least 80% of values are different
    if not (count >= n * 0.8) then
      goto doagain
    end
    -- min and max not too far from formal min and max
    local diff = (p2 - p1) >> 4
    if not (min < p1 + diff and max > p2 - diff) then
      goto doagain
    end
  end
  aux(0, maxint)
  aux(1, maxint)
  aux(3, maxint // 3)
  aux(minint, -1)
  aux(minint // 2, maxint // 2)
  aux(minint, maxint)
  aux(minint + 1, maxint)
  aux(minint, maxint - 1)
  aux(0, 1 << (intbits - 5))
end


assert(not pcall(random, 1, 2, 3))    -- too many arguments

-- empty interval
assert(not pcall(random, minint + 1, minint))
assert(not pcall(random, maxint, maxint - 1))
assert(not pcall(random, maxint, minint))



print('OK')
