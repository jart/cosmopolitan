-- $Id: testes/utf8.lua $
-- See Copyright Notice in file all.lua

print "testing UTF-8 library"

local utf8 = require'utf8'


local function checkerror (msg, f, ...)
  local s, err = pcall(f, ...)
  assert(not s and string.find(err, msg))
end


local function len (s)
  return #string.gsub(s, "[\x80-\xBF]", "")
end


local justone = "^" .. utf8.charpattern .. "$"

-- 't' is the list of codepoints of 's'
local function checksyntax (s, t)
  -- creates a string "return '\u{t[1]}...\u{t[n]}'"
  local ts = {"return '"}
  for i = 1, #t do ts[i + 1] = string.format("\\u{%x}", t[i]) end
  ts[#t + 2] = "'"
  ts = table.concat(ts)
  -- its execution should result in 's'
  assert(assert(load(ts))() == s)
end

assert(not utf8.offset("alo", 5))
assert(not utf8.offset("alo", -4))

-- 'check' makes several tests over the validity of string 's'.
-- 't' is the list of codepoints of 's'.
local function check (s, t, nonstrict)
  local l = utf8.len(s, 1, -1, nonstrict)
  assert(#t == l and len(s) == l)
  assert(utf8.char(table.unpack(t)) == s)   -- 't' and 's' are equivalent

  assert(utf8.offset(s, 0) == 1)

  checksyntax(s, t)

  -- creates new table with all codepoints of 's'
  local t1 = {utf8.codepoint(s, 1, -1, nonstrict)}
  assert(#t == #t1)
  for i = 1, #t do assert(t[i] == t1[i]) end   -- 't' is equal to 't1'

  for i = 1, l do   -- for all codepoints
    local pi = utf8.offset(s, i)        -- position of i-th char
    local pi1 = utf8.offset(s, 2, pi)   -- position of next char
    assert(string.find(string.sub(s, pi, pi1 - 1), justone))
    assert(utf8.offset(s, -1, pi1) == pi)
    assert(utf8.offset(s, i - l - 1) == pi)
    assert(pi1 - pi == #utf8.char(utf8.codepoint(s, pi, pi, nonstrict)))
    for j = pi, pi1 - 1 do
      assert(utf8.offset(s, 0, j) == pi)
    end
    for j = pi + 1, pi1 - 1 do
      assert(not utf8.len(s, j))
    end
   assert(utf8.len(s, pi, pi, nonstrict) == 1)
   assert(utf8.len(s, pi, pi1 - 1, nonstrict) == 1)
   assert(utf8.len(s, pi, -1, nonstrict) == l - i + 1)
   assert(utf8.len(s, pi1, -1, nonstrict) == l - i)
   assert(utf8.len(s, 1, pi, nonstrict) == i)
  end

  local i = 0
  for p, c in utf8.codes(s, nonstrict) do
    i = i + 1
    assert(c == t[i] and p == utf8.offset(s, i))
    assert(utf8.codepoint(s, p, p, nonstrict) == c)
  end
  assert(i == #t)

  i = 0
  for c in string.gmatch(s, utf8.charpattern) do
    i = i + 1
    assert(c == utf8.char(t[i]))
  end
  assert(i == #t)

  for i = 1, l do
    assert(utf8.offset(s, i) == utf8.offset(s, i - l - 1, #s + 1))
  end

end


do    -- error indication in utf8.len
  local function check (s, p)
    local a, b = utf8.len(s)
    assert(not a and b == p)
  end
  check("abc\xE3def", 4)
  check("汉字\x80", #("汉字") + 1)
  check("\xF4\x9F\xBF", 1)
  check("\xF4\x9F\xBF\xBF", 1)
end

-- errors in utf8.codes
do
  local function errorcodes (s)
    checkerror("invalid UTF%-8 code",
      function ()
        for c in utf8.codes(s) do assert(c) end
      end)
  end
  errorcodes("ab\xff")
  errorcodes("\u{110000}")
end

-- error in initial position for offset
checkerror("position out of bounds", utf8.offset, "abc", 1, 5)
checkerror("position out of bounds", utf8.offset, "abc", 1, -4)
checkerror("position out of bounds", utf8.offset, "", 1, 2)
checkerror("position out of bounds", utf8.offset, "", 1, -1)
checkerror("continuation byte", utf8.offset, "𦧺", 1, 2)
checkerror("continuation byte", utf8.offset, "𦧺", 1, 2)
checkerror("continuation byte", utf8.offset, "\x80", 1)

-- error in indices for len
checkerror("out of bounds", utf8.len, "abc", 0, 2)
checkerror("out of bounds", utf8.len, "abc", 1, 4)


local s = "hello World"
local t = {string.byte(s, 1, -1)}
for i = 1, utf8.len(s) do assert(t[i] == string.byte(s, i)) end
check(s, t)

check("汉字/漢字", {27721, 23383, 47, 28450, 23383,})

do
  local s = "áéí\128"
  local t = {utf8.codepoint(s,1,#s - 1)}
  assert(#t == 3 and t[1] == 225 and t[2] == 233 and t[3] == 237)
  checkerror("invalid UTF%-8 code", utf8.codepoint, s, 1, #s)
  checkerror("out of bounds", utf8.codepoint, s, #s + 1)
  t = {utf8.codepoint(s, 4, 3)}
  assert(#t == 0)
  checkerror("out of bounds", utf8.codepoint, s, -(#s + 1), 1)
  checkerror("out of bounds", utf8.codepoint, s, 1, #s + 1)
  -- surrogates
  assert(utf8.codepoint("\u{D7FF}") == 0xD800 - 1)
  assert(utf8.codepoint("\u{E000}") == 0xDFFF + 1)
  assert(utf8.codepoint("\u{D800}", 1, 1, true) == 0xD800)
  assert(utf8.codepoint("\u{DFFF}", 1, 1, true) == 0xDFFF)
  assert(utf8.codepoint("\u{7FFFFFFF}", 1, 1, true) == 0x7FFFFFFF)
end

assert(utf8.char() == "")
assert(utf8.char(0, 97, 98, 99, 1) == "\0abc\1")

assert(utf8.codepoint(utf8.char(0x10FFFF)) == 0x10FFFF)
assert(utf8.codepoint(utf8.char(0x7FFFFFFF), 1, 1, true) == (1<<31) - 1)

checkerror("value out of range", utf8.char, 0x7FFFFFFF + 1)
checkerror("value out of range", utf8.char, -1)

local function invalid (s)
  checkerror("invalid UTF%-8 code", utf8.codepoint, s)
  assert(not utf8.len(s))
end

-- UTF-8 representation for 0x11ffff (value out of valid range)
invalid("\xF4\x9F\xBF\xBF")

-- surrogates
invalid("\u{D800}")
invalid("\u{DFFF}")

-- overlong sequences
invalid("\xC0\x80")          -- zero
invalid("\xC1\xBF")          -- 0x7F (should be coded in 1 byte)
invalid("\xE0\x9F\xBF")      -- 0x7FF (should be coded in 2 bytes)
invalid("\xF0\x8F\xBF\xBF")  -- 0xFFFF (should be coded in 3 bytes)


-- invalid bytes
invalid("\x80")  -- continuation byte
invalid("\xBF")  -- continuation byte
invalid("\xFE")  -- invalid byte
invalid("\xFF")  -- invalid byte


-- empty string
check("", {})

-- minimum and maximum values for each sequence size
s = "\0 \x7F\z
     \xC2\x80 \xDF\xBF\z
     \xE0\xA0\x80 \xEF\xBF\xBF\z
     \xF0\x90\x80\x80  \xF4\x8F\xBF\xBF"
s = string.gsub(s, " ", "")
check(s, {0,0x7F, 0x80,0x7FF, 0x800,0xFFFF, 0x10000,0x10FFFF})

do
  -- original UTF-8 values
  local s = "\u{4000000}\u{7FFFFFFF}"
  assert(#s == 12)
  check(s, {0x4000000, 0x7FFFFFFF}, true)

  s = "\u{200000}\u{3FFFFFF}"
  assert(#s == 10)
  check(s, {0x200000, 0x3FFFFFF}, true)

  s = "\u{10000}\u{1fffff}"
  assert(#s == 8)
  check(s, {0x10000, 0x1FFFFF}, true)
end

x = "日本語a-4\0éó"
check(x, {26085, 26412, 35486, 97, 45, 52, 0, 233, 243})


-- Supplementary Characters
check("𣲷𠜎𠱓𡁻𠵼ab𠺢",
      {0x23CB7, 0x2070E, 0x20C53, 0x2107B, 0x20D7C, 0x61, 0x62, 0x20EA2,})

check("𨳊𩶘𦧺𨳒𥄫𤓓\xF4\x8F\xBF\xBF",
      {0x28CCA, 0x29D98, 0x269FA, 0x28CD2, 0x2512B, 0x244D3, 0x10ffff})


local i = 0
for p, c in string.gmatch(x, "()(" .. utf8.charpattern .. ")") do
  i = i + 1
  assert(utf8.offset(x, i) == p)
  assert(utf8.len(x, p) == utf8.len(x) - i + 1)
  assert(utf8.len(c) == 1)
  for j = 1, #c - 1 do
    assert(utf8.offset(x, 0, p + j - 1) == p)
  end
end

print'ok'

