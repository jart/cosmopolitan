-- $Id: test/strings.lua $
-- See Copyright Notice in file all.lua

print('testing strings and string library')

local maxi <const> = math.maxinteger
local mini <const> = math.mininteger


local function checkerror (msg, f, ...)
  local s, err = pcall(f, ...)
  assert(not s and string.find(err, msg))
end


-- testing string comparisons
assert('alo' < 'alo1')
assert('' < 'a')
assert('alo\0alo' < 'alo\0b')
assert('alo\0alo\0\0' > 'alo\0alo\0')
assert('alo' < 'alo\0')
assert('alo\0' > 'alo')
assert('\0' < '\1')
assert('\0\0' < '\0\1')
assert('\1\0a\0a' <= '\1\0a\0a')
assert(not ('\1\0a\0b' <= '\1\0a\0a'))
assert('\0\0\0' < '\0\0\0\0')
assert(not('\0\0\0\0' < '\0\0\0'))
assert('\0\0\0' <= '\0\0\0\0')
assert(not('\0\0\0\0' <= '\0\0\0'))
assert('\0\0\0' <= '\0\0\0')
assert('\0\0\0' >= '\0\0\0')
assert(not ('\0\0b' < '\0\0a\0'))

-- testing string.sub
assert(string.sub("123456789",2,4) == "234")
assert(string.sub("123456789",7) == "789")
assert(string.sub("123456789",7,6) == "")
assert(string.sub("123456789",7,7) == "7")
assert(string.sub("123456789",0,0) == "")
assert(string.sub("123456789",-10,10) == "123456789")
assert(string.sub("123456789",1,9) == "123456789")
assert(string.sub("123456789",-10,-20) == "")
assert(string.sub("123456789",-1) == "9")
assert(string.sub("123456789",-4) == "6789")
assert(string.sub("123456789",-6, -4) == "456")
assert(string.sub("123456789", mini, -4) == "123456")
assert(string.sub("123456789", mini, maxi) == "123456789")
assert(string.sub("123456789", mini, mini) == "")
assert(string.sub("\000123456789",3,5) == "234")
assert(("\000123456789"):sub(8) == "789")

-- testing string.find
assert(string.find("123456789", "345") == 3)
local a,b = string.find("123456789", "345")
assert(string.sub("123456789", a, b) == "345")
assert(string.find("1234567890123456789", "345", 3) == 3)
assert(string.find("1234567890123456789", "345", 4) == 13)
assert(not string.find("1234567890123456789", "346", 4))
assert(string.find("1234567890123456789", ".45", -9) == 13)
assert(not string.find("abcdefg", "\0", 5, 1))
assert(string.find("", "") == 1)
assert(string.find("", "", 1) == 1)
assert(not string.find("", "", 2))
assert(not string.find('', 'aaa', 1))
assert(('alo(.)alo'):find('(.)', 1, 1) == 4)

assert(string.len("") == 0)
assert(string.len("\0\0\0") == 3)
assert(string.len("1234567890") == 10)

assert(#"" == 0)
assert(#"\0\0\0" == 3)
assert(#"1234567890" == 10)

-- testing string.byte/string.char
assert(string.byte("a") == 97)
assert(string.byte("\xe4") > 127)
assert(string.byte(string.char(255)) == 255)
assert(string.byte(string.char(0)) == 0)
assert(string.byte("\0") == 0)
assert(string.byte("\0\0alo\0x", -1) == string.byte('x'))
assert(string.byte("ba", 2) == 97)
assert(string.byte("\n\n", 2, -1) == 10)
assert(string.byte("\n\n", 2, 2) == 10)
assert(string.byte("") == nil)
assert(string.byte("hi", -3) == nil)
assert(string.byte("hi", 3) == nil)
assert(string.byte("hi", 9, 10) == nil)
assert(string.byte("hi", 2, 1) == nil)
assert(string.char() == "")
assert(string.char(0, 255, 0) == "\0\255\0")
assert(string.char(0, string.byte("\xe4"), 0) == "\0\xe4\0")
assert(string.char(string.byte("\xe4l\0�u", 1, -1)) == "\xe4l\0�u")
assert(string.char(string.byte("\xe4l\0�u", 1, 0)) == "")
assert(string.char(string.byte("\xe4l\0�u", -10, 100)) == "\xe4l\0�u")

checkerror("out of range", string.char, 256)
checkerror("out of range", string.char, -1)
checkerror("out of range", string.char, math.maxinteger)
checkerror("out of range", string.char, math.mininteger)

assert(string.upper("ab\0c") == "AB\0C")
assert(string.lower("\0ABCc%$") == "\0abcc%$")
assert(string.rep('teste', 0) == '')
assert(string.rep('t�s\00t�', 2) == 't�s\0t�t�s\000t�')
assert(string.rep('', 10) == '')

if string.packsize("i") == 4 then
  -- result length would be 2^31 (int overflow)
  checkerror("too large", string.rep, 'aa', (1 << 30))
  checkerror("too large", string.rep, 'a', (1 << 30), ',')
end

-- repetitions with separator
assert(string.rep('teste', 0, 'xuxu') == '')
assert(string.rep('teste', 1, 'xuxu') == 'teste')
assert(string.rep('\1\0\1', 2, '\0\0') == '\1\0\1\0\0\1\0\1')
assert(string.rep('', 10, '.') == string.rep('.', 9))
assert(not pcall(string.rep, "aa", maxi // 2 + 10))
assert(not pcall(string.rep, "", maxi // 2 + 10, "aa"))

assert(string.reverse"" == "")
assert(string.reverse"\0\1\2\3" == "\3\2\1\0")
assert(string.reverse"\0001234" == "4321\0")

for i=0,30 do assert(string.len(string.rep('a', i)) == i) end

assert(type(tostring(nil)) == 'string')
assert(type(tostring(12)) == 'string')
assert(string.find(tostring{}, 'table:'))
assert(string.find(tostring(print), 'function:'))
assert(#tostring('\0') == 1)
assert(tostring(true) == "true")
assert(tostring(false) == "false")
assert(tostring(-1203) == "-1203")
assert(tostring(1203.125) == "1203.125")
assert(tostring(-0.5) == "-0.5")
assert(tostring(-32767) == "-32767")
if math.tointeger(2147483647) then   -- no overflow? (32 bits)
  assert(tostring(-2147483647) == "-2147483647")
end
if math.tointeger(4611686018427387904) then   -- no overflow? (64 bits)
  assert(tostring(4611686018427387904) == "4611686018427387904")
  assert(tostring(-4611686018427387904) == "-4611686018427387904")
end

if tostring(0.0) == "0.0" then   -- "standard" coercion float->string
  assert('' .. 12 == '12' and 12.0 .. '' == '12.0')
  assert(tostring(-1203 + 0.0) == "-1203.0")
else   -- compatible coercion
  assert(tostring(0.0) == "0")
  assert('' .. 12 == '12' and 12.0 .. '' == '12')
  assert(tostring(-1203 + 0.0) == "-1203")
end

do  -- tests for '%p' format
  -- not much to test, as C does not specify what '%p' does.
  -- ("The value of the pointer is converted to a sequence of printing
  -- characters, in an implementation-defined manner.")
  local null = "(null)"    -- nulls are formatted by Lua
  assert(string.format("%p", 4) == null)
  assert(string.format("%p", true) == null)
  assert(string.format("%p", nil) == null)
  assert(string.format("%p", {}) ~= null)
  assert(string.format("%p", print) ~= null)
  assert(string.format("%p", coroutine.running()) ~= null)
  assert(string.format("%p", io.stdin) ~= null)
  assert(string.format("%p", io.stdin) == string.format("%p", io.stdin))
  assert(string.format("%p", print) == string.format("%p", print))
  assert(string.format("%p", print) ~= string.format("%p", assert))

  assert(#string.format("%90p", {}) == 90)
  assert(#string.format("%-60p", {}) == 60)
  assert(string.format("%10p", false) == string.rep(" ", 10 - #null) .. null)
  assert(string.format("%-12p", 1.5) == null .. string.rep(" ", 12 - #null))

  do
    local t1 = {}; local t2 = {}
    assert(string.format("%p", t1) ~= string.format("%p", t2))
  end

  do     -- short strings are internalized
    local s1 = string.rep("a", 10)
    local s2 = string.rep("aa", 5)
  assert(string.format("%p", s1) == string.format("%p", s2))
  end

  do     -- long strings aren't internalized
    local s1 = string.rep("a", 300); local s2 = string.rep("a", 300)
    assert(string.format("%p", s1) ~= string.format("%p", s2))
  end
end

local x = '"�lo"\n\\'
assert(string.format('%q%s', x, x) == '"\\"�lo\\"\\\n\\\\""�lo"\n\\')
assert(string.format('%q', "\0") == [["\0"]])
assert(load(string.format('return %q', x))() == x)
x = "\0\1\0023\5\0009"
assert(load(string.format('return %q', x))() == x)
assert(string.format("\0%c\0%c%x\0", string.byte("\xe4"), string.byte("b"), 140) ==
              "\0\xe4\0b8c\0")
assert(string.format('') == "")
assert(string.format("%c",34)..string.format("%c",48)..string.format("%c",90)..string.format("%c",100) ==
       string.format("%1c%-c%-1c%c", 34, 48, 90, 100))
assert(string.format("%s\0 is not \0%s", 'not be', 'be') == 'not be\0 is not \0be')
assert(string.format("%%%d %010d", 10, 23) == "%10 0000000023")
assert(tonumber(string.format("%f", 10.3)) == 10.3)
assert(string.format('"%-50s"', 'a') == '"a' .. string.rep(' ', 49) .. '"')

assert(string.format("-%.20s.20s", string.rep("%", 2000)) ==
                     "-"..string.rep("%", 20)..".20s")
assert(string.format('"-%20s.20s"', string.rep("%", 2000)) ==
       string.format("%q", "-"..string.rep("%", 2000)..".20s"))

do
  local function checkQ (v)
    local s = string.format("%q", v)
    local nv = load("return " .. s)()
    assert(v == nv and math.type(v) == math.type(nv))
  end
  checkQ("\0\0\1\255\u{234}")
  checkQ(math.maxinteger)
  checkQ(math.mininteger)
  checkQ(math.pi)
  checkQ(0.1)
  checkQ(true)
  checkQ(nil)
  checkQ(false)
  checkQ(math.huge)
  checkQ(-math.huge)
  assert(string.format("%q", 0/0) == "(0/0)")   -- NaN
  checkerror("no literal", string.format, "%q", {})
end

assert(string.format("\0%s\0", "\0\0\1") == "\0\0\0\1\0")
checkerror("contains zeros", string.format, "%10s", "\0")

-- format x tostring
assert(string.format("%s %s", nil, true) == "nil true")
assert(string.format("%s %.4s", false, true) == "false true")
assert(string.format("%.3s %.3s", false, true) == "fal tru")
local m = setmetatable({}, {__tostring = function () return "hello" end,
                            __name = "hi"})
assert(string.format("%s %.10s", m, m) == "hello hello")
getmetatable(m).__tostring = nil   -- will use '__name' from now on
assert(string.format("%.4s", m) == "hi: ")

getmetatable(m).__tostring = function () return {} end
checkerror("'__tostring' must return a string", tostring, m)


assert(string.format("%x", 0.0) == "0")
assert(string.format("%02x", 0.0) == "00")
assert(string.format("%08X", 0xFFFFFFFF) == "FFFFFFFF")
assert(string.format("%+08d", 31501) == "+0031501")
assert(string.format("%+08d", -30927) == "-0030927")


do    -- longest number that can be formatted
  local i = 1
  local j = 10000
  while i + 1 < j do   -- binary search for maximum finite float
    local m = (i + j) // 2
    if 10^m < math.huge then i = m else j = m end
  end
  assert(10^i < math.huge and 10^j == math.huge)
  local s = string.format('%.99f', -(10^i))
  assert(string.len(s) >= i + 101)
  assert(tonumber(s) == -(10^i))

  -- limit for floats
  assert(10^38 < math.huge)
  local s = string.format('%.99f', -(10^38))
  assert(string.len(s) >= 38 + 101)
  assert(tonumber(s) == -(10^38))
end


-- testing large numbers for format
do   -- assume at least 32 bits
  local max, min = 0x7fffffff, -0x80000000    -- "large" for 32 bits
  assert(string.sub(string.format("%8x", -1), -8) == "ffffffff")
  assert(string.format("%x", max) == "7fffffff")
  assert(string.sub(string.format("%x", min), -8) == "80000000")
  assert(string.format("%d", max) ==  "2147483647")
  assert(string.format("%d", min) == "-2147483648")
  assert(string.format("%u", 0xffffffff) == "4294967295")
  assert(string.format("%o", 0xABCD) == "125715")

  max, min = 0x7fffffffffffffff, -0x8000000000000000
  if max > 2.0^53 then  -- only for 64 bits
    assert(string.format("%x", (2^52 | 0) - 1) == "fffffffffffff")
    assert(string.format("0x%8X", 0x8f000003) == "0x8F000003")
    assert(string.format("%d", 2^53) == "9007199254740992")
    assert(string.format("%i", -2^53) == "-9007199254740992")
    assert(string.format("%x", max) == "7fffffffffffffff")
    assert(string.format("%x", min) == "8000000000000000")
    assert(string.format("%d", max) ==  "9223372036854775807")
    assert(string.format("%d", min) == "-9223372036854775808")
    assert(string.format("%u", ~(-1 << 64)) == "18446744073709551615")
    assert(tostring(1234567890123) == '1234567890123')
  end
end


do print("testing 'format %a %A'")
  local function matchhexa (n)
    local s = string.format("%a", n)
    -- result matches ISO C requirements
    assert(string.find(s, "^%-?0x[1-9a-f]%.?[0-9a-f]*p[-+]?%d+$"))
    assert(tonumber(s) == n)  -- and has full precision
    s = string.format("%A", n)
    assert(string.find(s, "^%-?0X[1-9A-F]%.?[0-9A-F]*P[-+]?%d+$"))
    assert(tonumber(s) == n)
  end
  for _, n in ipairs{0.1, -0.1, 1/3, -1/3, 1e30, -1e30,
                     -45/247, 1, -1, 2, -2, 3e-20, -3e-20} do
    matchhexa(n)
  end

  assert(string.find(string.format("%A", 0.0), "^0X0%.?0*P%+?0$"))
  assert(string.find(string.format("%a", -0.0), "^%-0x0%.?0*p%+?0$"))

  if not _port then   -- test inf, -inf, NaN, and -0.0
    assert(string.find(string.format("%a", 1/0), "^inf"))
    assert(string.find(string.format("%A", -1/0), "^%-INF"))
    assert(string.find(string.format("%a", 0/0), "^%-?nan"))
    assert(string.find(string.format("%a", -0.0), "^%-0x0"))
  end
  
  if not pcall(string.format, "%.3a", 0) then
    (Message or print)("\n >>> modifiers for format '%a' not available <<<\n")
  else
    assert(string.find(string.format("%+.2A", 12), "^%+0X%x%.%x0P%+?%d$"))
    assert(string.find(string.format("%.4A", -12), "^%-0X%x%.%x000P%+?%d$"))
  end
end


-- testing some flags  (all these results are required by ISO C)
assert(string.format("%#12o", 10) == "         012")
assert(string.format("%#10x", 100) == "      0x64")
assert(string.format("%#-17X", 100) == "0X64             ")
assert(string.format("%013i", -100) == "-000000000100")
assert(string.format("%2.5d", -100) == "-00100")
assert(string.format("%.u", 0) == "")
assert(string.format("%+#014.0f", 100) == "+000000000100.")
assert(string.format("%-16c", 97) == "a               ")
assert(string.format("%+.3G", 1.5) == "+1.5")
assert(string.format("%.0s", "alo")  == "")
assert(string.format("%.s", "alo")  == "")

-- ISO C89 says that "The exponent always contains at least two digits",
-- but unlike ISO C99 it does not ensure that it contains "only as many
-- more digits as necessary".
assert(string.match(string.format("% 1.0E", 100), "^ 1E%+0+2$"))
assert(string.match(string.format("% .1g", 2^10), "^ 1e%+0+3$"))


-- errors in format

local function check (fmt, msg)
  checkerror(msg, string.format, fmt, 10)
end

local aux = string.rep('0', 600)
check("%100.3d", "invalid conversion")
check("%1"..aux..".3d", "too long")
check("%1.100d", "invalid conversion")
check("%10.1"..aux.."004d", "too long")
check("%t", "invalid conversion")
check("%"..aux.."d", "too long")
check("%d %d", "no value")
check("%010c", "invalid conversion")
check("%.10c", "invalid conversion")
check("%0.34s", "invalid conversion")
check("%#i", "invalid conversion")
check("%3.1p", "invalid conversion")
check("%0.s", "invalid conversion")
check("%10q", "cannot have modifiers")
check("%F", "invalid conversion")   -- useless and not in C89


assert(load("return 1\n--comment without ending EOL")() == 1)


checkerror("table expected", table.concat, 3)
checkerror("at index " .. maxi, table.concat, {}, " ", maxi, maxi)
-- '%' escapes following minus signal
checkerror("at index %" .. mini, table.concat, {}, " ", mini, mini)
assert(table.concat{} == "")
assert(table.concat({}, 'x') == "")
assert(table.concat({'\0', '\0\1', '\0\1\2'}, '.\0.') == "\0.\0.\0\1.\0.\0\1\2")
local a = {}; for i=1,300 do a[i] = "xuxu" end
assert(table.concat(a, "123").."123" == string.rep("xuxu123", 300))
assert(table.concat(a, "b", 20, 20) == "xuxu")
assert(table.concat(a, "", 20, 21) == "xuxuxuxu")
assert(table.concat(a, "x", 22, 21) == "")
assert(table.concat(a, "3", 299) == "xuxu3xuxu")
assert(table.concat({}, "x", maxi, maxi - 1) == "")
assert(table.concat({}, "x", mini + 1, mini) == "")
assert(table.concat({}, "x", maxi, mini) == "")
assert(table.concat({[maxi] = "alo"}, "x", maxi, maxi) == "alo")
assert(table.concat({[maxi] = "alo", [maxi - 1] = "y"}, "-", maxi - 1, maxi)
       == "y-alo")

assert(not pcall(table.concat, {"a", "b", {}}))

a = {"a","b","c"}
assert(table.concat(a, ",", 1, 0) == "")
assert(table.concat(a, ",", 1, 1) == "a")
assert(table.concat(a, ",", 1, 2) == "a,b")
assert(table.concat(a, ",", 2) == "b,c")
assert(table.concat(a, ",", 3) == "c")
assert(table.concat(a, ",", 4) == "")

if not _port then

  local locales = { "ptb", "pt_BR.iso88591", "ISO-8859-1" }
  local function trylocale (w)
    for i = 1, #locales do
      if os.setlocale(locales[i], w) then
        print(string.format("'%s' locale set to '%s'", w, locales[i]))
        return locales[i]
      end
    end
    print(string.format("'%s' locale not found", w))
    return false
  end

  if trylocale("collate")  then
    -- <disabled by jart: doesn't play nice with musl locale faking>
    -- assert("alo" < "�lo" and "�lo" < "amo")
    -- </disabled by jart>
  end

  if trylocale("ctype") then
    -- <disabled by jart: doesn't play nice with musl locale faking>
    -- assert(string.gsub("�����", "%a", "x") == "xxxxx")
    -- assert(string.gsub("����", "%l", "x") == "x�x�")
    -- assert(string.gsub("����", "%u", "x") == "�x�x")
    -- assert(string.upper"���{xuxu}��o" == "���{XUXU}��O")
    -- </disabled by jart>
  end

  os.setlocale("C")
  assert(os.setlocale() == 'C')
  assert(os.setlocale(nil, "numeric") == 'C')

end


-- bug in Lua 5.3.2
-- 'gmatch' iterator does not work across coroutines
do
  local f = string.gmatch("1 2 3 4 5", "%d+")
  assert(f() == "1")
  local co = coroutine.wrap(f)
  assert(co() == "2")
end


if T==nil then
  (Message or print)
     ("\n >>> testC not active: skipping 'pushfstring' tests <<<\n")
else

  print"testing 'pushfstring'"

  -- formats %U, %f, %I already tested elsewhere

  local blen = 200    -- internal buffer length in 'luaO_pushfstring'

  local function callpfs (op, fmt, n)
    local x = {T.testC("pushfstring" .. op .. "; return *", fmt, n)}
    -- stack has code, 'fmt', 'n', and result from operation
    assert(#x == 4)  -- make sure nothing else was left in the stack
    return x[4]
  end

  local function testpfs (op, fmt, n)
    assert(callpfs(op, fmt, n) == string.format(fmt, n))
  end

  testpfs("I", "", 0)
  testpfs("I", string.rep("a", blen - 1), 0)
  testpfs("I", string.rep("a", blen), 0)
  testpfs("I", string.rep("a", blen + 1), 0)

  local str = string.rep("ab", blen) .. "%d" .. string.rep("d", blen / 2)
  testpfs("I", str, 2^14)
  testpfs("I", str, -2^15)

  str = "%d" .. string.rep("cd", blen)
  testpfs("I", str, 2^14)
  testpfs("I", str, -2^15)

  str = string.rep("c", blen - 2) .. "%d"
  testpfs("I", str, 2^14)
  testpfs("I", str, -2^15)

  for l = 12, 14 do
    local str1 = string.rep("a", l)
    for i = 0, 500, 13 do
      for j = 0, 500, 13 do
        str = string.rep("a", i) .. "%s" .. string.rep("d", j)
        testpfs("S", str, str1)
        testpfs("S", str, str)
      end
    end
  end

  str = "abc %c def"
  testpfs("I", str, string.byte("A"))
  testpfs("I", str, 255)

  str = string.rep("a", blen - 1) .. "%p" .. string.rep("cd", blen)
  testpfs("P", str, {})

  str = string.rep("%%", 3 * blen) .. "%p" .. string.rep("%%", 2 * blen)
  testpfs("P", str, {})
end


print('OK')

