-- $Id: testes/pm.lua $
-- See Copyright Notice in file all.lua

print('testing pattern matching')

local function checkerror (msg, f, ...)
  local s, err = pcall(f, ...)
  assert(not s and string.find(err, msg))
end


function f(s, p)
  local i,e = string.find(s, p)
  if i then return string.sub(s, i, e) end
end

a,b = string.find('', '')    -- empty patterns are tricky
assert(a == 1 and b == 0);
a,b = string.find('alo', '')
assert(a == 1 and b == 0)
a,b = string.find('a\0o a\0o a\0o', 'a', 1)   -- first position
assert(a == 1 and b == 1)
a,b = string.find('a\0o a\0o a\0o', 'a\0o', 2)   -- starts in the midle
assert(a == 5 and b == 7)
a,b = string.find('a\0o a\0o a\0o', 'a\0o', 9)   -- starts in the midle
assert(a == 9 and b == 11)
a,b = string.find('a\0a\0a\0a\0\0ab', '\0ab', 2);  -- finds at the end
assert(a == 9 and b == 11);
a,b = string.find('a\0a\0a\0a\0\0ab', 'b')    -- last position
assert(a == 11 and b == 11)
assert(not string.find('a\0a\0a\0a\0\0ab', 'b\0'))   -- check ending
assert(not string.find('', '\0'))
assert(string.find('alo123alo', '12') == 4)
assert(not string.find('alo123alo', '^12'))

assert(string.match("aaab", ".*b") == "aaab")
assert(string.match("aaa", ".*a") == "aaa")
assert(string.match("b", ".*b") == "b")

assert(string.match("aaab", ".+b") == "aaab")
assert(string.match("aaa", ".+a") == "aaa")
assert(not string.match("b", ".+b"))

assert(string.match("aaab", ".?b") == "ab")
assert(string.match("aaa", ".?a") == "aa")
assert(string.match("b", ".?b") == "b")

assert(f('aloALO', '%l*') == 'alo')
assert(f('aLo_ALO', '%a*') == 'aLo')

assert(f("  \n\r*&\n\r   xuxu  \n\n", "%g%g%g+") == "xuxu")

assert(f('aaab', 'a*') == 'aaa');
assert(f('aaa', '^.*$') == 'aaa');
assert(f('aaa', 'b*') == '');
assert(f('aaa', 'ab*a') == 'aa')
assert(f('aba', 'ab*a') == 'aba')
assert(f('aaab', 'a+') == 'aaa')
assert(f('aaa', '^.+$') == 'aaa')
assert(not f('aaa', 'b+'))
assert(not f('aaa', 'ab+a'))
assert(f('aba', 'ab+a') == 'aba')
assert(f('a$a', '.$') == 'a')
assert(f('a$a', '.%$') == 'a$')
assert(f('a$a', '.$.') == 'a$a')
assert(not f('a$a', '$$'))
assert(not f('a$b', 'a$'))
assert(f('a$a', '$') == '')
assert(f('', 'b*') == '')
assert(not f('aaa', 'bb*'))
assert(f('aaab', 'a-') == '')
assert(f('aaa', '^.-$') == 'aaa')
assert(f('aabaaabaaabaaaba', 'b.*b') == 'baaabaaabaaab')
assert(f('aabaaabaaabaaaba', 'b.-b') == 'baaab')
assert(f('alo xo', '.o$') == 'xo')
assert(f(' \n isto é assim', '%S%S*') == 'isto')
assert(f(' \n isto é assim', '%S*$') == 'assim')
assert(f(' \n isto é assim', '[a-z]*$') == 'assim')
assert(f('um caracter ? extra', '[^%sa-z]') == '?')
assert(f('', 'a?') == '')
assert(f('á', 'á?') == 'á')
assert(f('ábl', 'á?b?l?') == 'ábl')
assert(f('  ábl', 'á?b?l?') == '')
assert(f('aa', '^aa?a?a') == 'aa')
assert(f(']]]áb', '[^]]') == 'á')
assert(f("0alo alo", "%x*") == "0a")
assert(f("alo alo", "%C+") == "alo alo")
print('+')


function f1(s, p)
  p = string.gsub(p, "%%([0-9])", function (s)
        return "%" .. (tonumber(s)+1)
       end)
  p = string.gsub(p, "^(^?)", "%1()", 1)
  p = string.gsub(p, "($?)$", "()%1", 1)
  local t = {string.match(s, p)}
  return string.sub(s, t[1], t[#t] - 1)
end

assert(f1('alo alx 123 b\0o b\0o', '(..*) %1') == "b\0o b\0o")
assert(f1('axz123= 4= 4 34', '(.+)=(.*)=%2 %1') == '3= 4= 4 3')
assert(f1('=======', '^(=*)=%1$') == '=======')
assert(not string.match('==========', '^([=]*)=%1$'))

local function range (i, j)
  if i <= j then
    return i, range(i+1, j)
  end
end

local abc = string.char(range(0, 127)) .. string.char(range(128, 255));

assert(string.len(abc) == 256)

function strset (p)
  local res = {s=''}
  string.gsub(abc, p, function (c) res.s = res.s .. c end)
  return res.s
end;

assert(string.len(strset('[\200-\210]')) == 11)

assert(strset('[a-z]') == "abcdefghijklmnopqrstuvwxyz")
assert(strset('[a-z%d]') == strset('[%da-uu-z]'))
assert(strset('[a-]') == "-a")
assert(strset('[^%W]') == strset('[%w]'))
assert(strset('[]%%]') == '%]')
assert(strset('[a%-z]') == '-az')
assert(strset('[%^%[%-a%]%-b]') == '-[]^ab')
assert(strset('%Z') == strset('[\1-\255]'))
assert(strset('.') == strset('[\1-\255%z]'))
print('+');

assert(string.match("alo xyzK", "(%w+)K") == "xyz")
assert(string.match("254 K", "(%d*)K") == "")
assert(string.match("alo ", "(%w*)$") == "")
assert(not string.match("alo ", "(%w+)$"))
assert(string.find("(álo)", "%(á") == 1)
local a, b, c, d, e = string.match("âlo alo", "^(((.).).* (%w*))$")
assert(a == 'âlo alo' and b == 'âl' and c == 'â' and d == 'alo' and e == nil)
a, b, c, d  = string.match('0123456789', '(.+(.?)())')
assert(a == '0123456789' and b == '' and c == 11 and d == nil)
print('+')

assert(string.gsub('ülo ülo', 'ü', 'x') == 'xlo xlo')
assert(string.gsub('alo úlo  ', ' +$', '') == 'alo úlo')  -- trim
assert(string.gsub('  alo alo  ', '^%s*(.-)%s*$', '%1') == 'alo alo')  -- double trim
assert(string.gsub('alo  alo  \n 123\n ', '%s+', ' ') == 'alo alo 123 ')
t = "abç d"
a, b = string.gsub(t, '(.)', '%1@')
assert('@'..a == string.gsub(t, '', '@') and b == 5)
a, b = string.gsub('abçd', '(.)', '%0@', 2)
assert(a == 'a@b@çd' and b == 2)
assert(string.gsub('alo alo', '()[al]', '%1') == '12o 56o')
assert(string.gsub("abc=xyz", "(%w*)(%p)(%w+)", "%3%2%1-%0") ==
              "xyz=abc-abc=xyz")
assert(string.gsub("abc", "%w", "%1%0") == "aabbcc")
assert(string.gsub("abc", "%w+", "%0%1") == "abcabc")
assert(string.gsub('áéí', '$', '\0óú') == 'áéí\0óú')
assert(string.gsub('', '^', 'r') == 'r')
assert(string.gsub('', '$', 'r') == 'r')
print('+')


do   -- new (5.3.3) semantics for empty matches
  assert(string.gsub("a b cd", " *", "-") == "-a-b-c-d-")

  local res = ""
  local sub = "a  \nbc\t\td"
  local i = 1
  for p, e in string.gmatch(sub, "()%s*()") do
    res = res .. string.sub(sub, i, p - 1) .. "-"
    i = e
  end
  assert(res == "-a-b-c-d-")
end


assert(string.gsub("um (dois) tres (quatro)", "(%(%w+%))", string.upper) ==
            "um (DOIS) tres (QUATRO)")

do
  local function setglobal (n,v) rawset(_G, n, v) end
  string.gsub("a=roberto,roberto=a", "(%w+)=(%w%w*)", setglobal)
  assert(_G.a=="roberto" and _G.roberto=="a")
end

function f(a,b) return string.gsub(a,'.',b) end
assert(string.gsub("trocar tudo em |teste|b| é |beleza|al|", "|([^|]*)|([^|]*)|", f) ==
            "trocar tudo em bbbbb é alalalalalal")

local function dostring (s) return load(s, "")() or "" end
assert(string.gsub("alo $a='x'$ novamente $return a$",
                   "$([^$]*)%$",
                   dostring) == "alo  novamente x")

x = string.gsub("$x=string.gsub('alo', '.', string.upper)$ assim vai para $return x$",
         "$([^$]*)%$", dostring)
assert(x == ' assim vai para ALO')

t = {}
s = 'a alo jose  joao'
r = string.gsub(s, '()(%w+)()', function (a,w,b)
      assert(string.len(w) == b-a);
      t[a] = b-a;
    end)
assert(s == r and t[1] == 1 and t[3] == 3 and t[7] == 4 and t[13] == 4)


function isbalanced (s)
  return not string.find(string.gsub(s, "%b()", ""), "[()]")
end

assert(isbalanced("(9 ((8))(\0) 7) \0\0 a b ()(c)() a"))
assert(not isbalanced("(9 ((8) 7) a b (\0 c) a"))
assert(string.gsub("alo 'oi' alo", "%b''", '"') == 'alo " alo')


local t = {"apple", "orange", "lime"; n=0}
assert(string.gsub("x and x and x", "x", function () t.n=t.n+1; return t[t.n] end)
        == "apple and orange and lime")

t = {n=0}
string.gsub("first second word", "%w%w*", function (w) t.n=t.n+1; t[t.n] = w end)
assert(t[1] == "first" and t[2] == "second" and t[3] == "word" and t.n == 3)

t = {n=0}
assert(string.gsub("first second word", "%w+",
         function (w) t.n=t.n+1; t[t.n] = w end, 2) == "first second word")
assert(t[1] == "first" and t[2] == "second" and t[3] == undef)

checkerror("invalid replacement value %(a table%)",
            string.gsub, "alo", ".", {a = {}})
checkerror("invalid capture index %%2", string.gsub, "alo", ".", "%2")
checkerror("invalid capture index %%0", string.gsub, "alo", "(%0)", "a")
checkerror("invalid capture index %%1", string.gsub, "alo", "(%1)", "a")
checkerror("invalid use of '%%'", string.gsub, "alo", ".", "%x")


if not _soft then
  print("big strings")
  local a = string.rep('a', 300000)
  assert(string.find(a, '^a*.?$'))
  assert(not string.find(a, '^a*.?b$'))
  assert(string.find(a, '^a-.?$'))

  -- bug in 5.1.2
  a = string.rep('a', 10000) .. string.rep('b', 10000)
  assert(not pcall(string.gsub, a, 'b'))
end

-- recursive nest of gsubs
function rev (s)
  return string.gsub(s, "(.)(.+)", function (c,s1) return rev(s1)..c end)
end

local x = "abcdef"
assert(rev(rev(x)) == x)


-- gsub with tables
assert(string.gsub("alo alo", ".", {}) == "alo alo")
assert(string.gsub("alo alo", "(.)", {a="AA", l=""}) == "AAo AAo")
assert(string.gsub("alo alo", "(.).", {a="AA", l="K"}) == "AAo AAo")
assert(string.gsub("alo alo", "((.)(.?))", {al="AA", o=false}) == "AAo AAo")

assert(string.gsub("alo alo", "().", {'x','yy','zzz'}) == "xyyzzz alo")

t = {}; setmetatable(t, {__index = function (t,s) return string.upper(s) end})
assert(string.gsub("a alo b hi", "%w%w+", t) == "a ALO b HI")


-- tests for gmatch
local a = 0
for i in string.gmatch('abcde', '()') do assert(i == a+1); a=i end
assert(a==6)

t = {n=0}
for w in string.gmatch("first second word", "%w+") do
      t.n=t.n+1; t[t.n] = w
end
assert(t[1] == "first" and t[2] == "second" and t[3] == "word")

t = {3, 6, 9}
for i in string.gmatch ("xuxx uu ppar r", "()(.)%2") do
  assert(i == table.remove(t, 1))
end
assert(#t == 0)

t = {}
for i,j in string.gmatch("13 14 10 = 11, 15= 16, 22=23", "(%d+)%s*=%s*(%d+)") do
  t[tonumber(i)] = tonumber(j)
end
a = 0
for k,v in pairs(t) do assert(k+1 == v+0); a=a+1 end
assert(a == 3)


do   -- init parameter in gmatch
  local s = 0
  for k in string.gmatch("10 20 30", "%d+", 3) do
    s = s + tonumber(k)
  end
  assert(s == 50)

  s = 0
  for k in string.gmatch("11 21 31", "%d+", -4) do
    s = s + tonumber(k)
  end
  assert(s == 32)

  -- there is an empty string at the end of the subject
  s = 0
  for k in string.gmatch("11 21 31", "%w*", 9) do
    s = s + 1
  end
  assert(s == 1)

  -- there are no empty strings after the end of the subject
  s = 0
  for k in string.gmatch("11 21 31", "%w*", 10) do
    s = s + 1
  end
  assert(s == 0)
end


-- tests for `%f' (`frontiers')

assert(string.gsub("aaa aa a aaa a", "%f[%w]a", "x") == "xaa xa x xaa x")
assert(string.gsub("[[]] [][] [[[[", "%f[[].", "x") == "x[]] x]x] x[[[")
assert(string.gsub("01abc45de3", "%f[%d]", ".") == ".01abc.45de.3")
assert(string.gsub("01abc45 de3x", "%f[%D]%w", ".") == "01.bc45 de3.")
assert(string.gsub("function", "%f[\1-\255]%w", ".") == ".unction")
assert(string.gsub("function", "%f[^\1-\255]", ".") == "function.")

assert(string.find("a", "%f[a]") == 1)
assert(string.find("a", "%f[^%z]") == 1)
assert(string.find("a", "%f[^%l]") == 2)
assert(string.find("aba", "%f[a%z]") == 3)
assert(string.find("aba", "%f[%z]") == 4)
assert(not string.find("aba", "%f[%l%z]"))
assert(not string.find("aba", "%f[^%l%z]"))

local i, e = string.find(" alo aalo allo", "%f[%S].-%f[%s].-%f[%S]")
assert(i == 2 and e == 5)
local k = string.match(" alo aalo allo", "%f[%S](.-%f[%s].-%f[%S])")
assert(k == 'alo ')

local a = {1, 5, 9, 14, 17,}
for k in string.gmatch("alo alo th02 is 1hat", "()%f[%w%d]") do
  assert(table.remove(a, 1) == k)
end
assert(#a == 0)


-- malformed patterns
local function malform (p, m)
  m = m or "malformed"
  local r, msg = pcall(string.find, "a", p)
  assert(not r and string.find(msg, m))
end

malform("(.", "unfinished capture")
malform(".)", "invalid pattern capture")
malform("[a")
malform("[]")
malform("[^]")
malform("[a%]")
malform("[a%")
malform("%b")
malform("%ba")
malform("%")
malform("%f", "missing")

-- \0 in patterns
assert(string.match("ab\0\1\2c", "[\0-\2]+") == "\0\1\2")
assert(string.match("ab\0\1\2c", "[\0-\0]+") == "\0")
assert(string.find("b$a", "$\0?") == 2)
assert(string.find("abc\0efg", "%\0") == 4)
assert(string.match("abc\0efg\0\1e\1g", "%b\0\1") == "\0efg\0\1e\1")
assert(string.match("abc\0\0\0", "%\0+") == "\0\0\0")
assert(string.match("abc\0\0\0", "%\0%\0?") == "\0\0")

-- magic char after \0
assert(string.find("abc\0\0","\0.") == 4)
assert(string.find("abcx\0\0abc\0abc","x\0\0abc\0a.") == 4)


do   -- test reuse of original string in gsub
  local s = string.rep("a", 100)
  local r = string.gsub(s, "b", "c")   -- no match
  assert(string.format("%p", s) == string.format("%p", r))

  r = string.gsub(s, ".", {x = "y"})   -- no substitutions
  assert(string.format("%p", s) == string.format("%p", r))

  local count = 0
  r = string.gsub(s, ".", function (x)
                            assert(x == "a")
                            count = count + 1
                            return nil    -- no substitution
                          end)
  r = string.gsub(r, ".", {b = 'x'})   -- "a" is not a key; no subst.
  assert(count == 100)
  assert(string.format("%p", s) == string.format("%p", r))

  count = 0
  r = string.gsub(s, ".", function (x)
                            assert(x == "a")
                            count = count + 1
                            return x    -- substitution...
                          end)
  assert(count == 100)
  -- no reuse in this case
  assert(r == s and string.format("%p", s) ~= string.format("%p", r))
end

print('OK')

