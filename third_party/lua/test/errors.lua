-- $Id: test/errors.lua $
-- See Copyright Notice in file all.lua

print("testing errors")

local debug = require"debug"

-- avoid problems with 'strict' module (which may generate other error messages)
local mt = getmetatable(_G) or {}
local oldmm = mt.__index
mt.__index = nil

local function checkerr (msg, f, ...)
  local st, err = pcall(f, ...)
  assert(not st and string.find(err, msg))
end


local function doit (s)
  local f, msg = load(s)
  if not f then return msg end
  local cond, msg = pcall(f)
  return (not cond) and msg
end


local function checkmessage (prog, msg, debug)
  local m = doit(prog)
  if debug then print(m) end
  assert(string.find(m, msg, 1, true))
end

local function checksyntax (prog, extra, token, line)
  local msg = doit(prog)
  if not string.find(token, "^<%a") and not string.find(token, "^char%(")
    then token = "'"..token.."'" end
  token = string.gsub(token, "(%p)", "%%%1")
  local pt = string.format([[^%%[string ".*"%%]:%d: .- near %s$]],
                           line, token)
  assert(string.find(msg, pt))
  assert(string.find(msg, msg, 1, true))
end


-- test error message with no extra info
assert(doit("error('hi', 0)") == 'hi')

-- test error message with no info
assert(doit("error()") == nil)


-- test common errors/errors that crashed in the past
assert(doit("table.unpack({}, 1, n=2^30)"))
assert(doit("a=math.sin()"))
assert(not doit("tostring(1)") and doit("tostring()"))
assert(doit"tonumber()")
assert(doit"repeat until 1; a")
assert(doit"return;;")
assert(doit"assert(false)")
assert(doit"assert(nil)")
assert(doit("function a (... , ...) end"))
assert(doit("function a (, ...) end"))
assert(doit("local t={}; t = t[#t] + 1"))

checksyntax([[
  local a = {4

]], "'}' expected (to close '{' at line 1)", "<eof>", 3)


do   -- testing errors in goto/break
  local function checksyntax (prog, msg, line)
    local st, err = load(prog)
    assert(string.find(err, "line " .. line))
    assert(string.find(err, msg, 1, true))
  end

  checksyntax([[
    ::A:: a = 1
    ::A::
  ]], "label 'A' already defined", 1)

  checksyntax([[
    a = 1
    goto A
    do ::A:: end
  ]], "no visible label 'A'", 2)

end


if not T then
  (Message or print)
    ('\n >>> testC not active: skipping memory message test <<<\n')
else
  print "testing memory error message"
  local a = {}
  for i = 1, 10000 do a[i] = true end   -- preallocate array
  collectgarbage()
  T.totalmem(T.totalmem() + 10000)
  -- force a memory error (by a small margin)
  local st, msg = pcall(function()
    for i = 1, 100000 do a[i] = tostring(i) end
  end)
  T.totalmem(0)
  assert(not st and msg == "not enough" .. " memory")
end


-- tests for better error messages

checkmessage("a = {} + 1", "arithmetic")
checkmessage("a = {} | 1", "bitwise operation")
checkmessage("a = {} < 1", "attempt to compare")
checkmessage("a = {} <= 1", "attempt to compare")

checkmessage("a=1; bbbb=2; a=math.sin(3)+bbbb(3)", "global 'bbbb'")
checkmessage("a={}; do local a=1 end a:bbbb(3)", "method 'bbbb'")
checkmessage("local a={}; a.bbbb(3)", "field 'bbbb'")
assert(not string.find(doit"a={13}; local bbbb=1; a[bbbb](3)", "'bbbb'"))
checkmessage("a={13}; local bbbb=1; a[bbbb](3)", "number")
checkmessage("a=(1)..{}", "a table value")

-- calls
checkmessage("local a; a(13)", "local 'a'")
checkmessage([[
  local a = setmetatable({}, {__add = 34})
  a = a + 1
]], "metamethod 'add'")
checkmessage([[
  local a = setmetatable({}, {__lt = {}})
  a = a > a
]], "metamethod 'lt'")

-- tail calls
checkmessage("local a={}; return a.bbbb(3)", "field 'bbbb'")
checkmessage("a={}; do local a=1 end; return a:bbbb(3)", "method 'bbbb'")

checkmessage("a = #print", "length of a function value")
checkmessage("a = #3", "length of a number value")

aaa = nil
checkmessage("aaa.bbb:ddd(9)", "global 'aaa'")
checkmessage("local aaa={bbb=1}; aaa.bbb:ddd(9)", "field 'bbb'")
checkmessage("local aaa={bbb={}}; aaa.bbb:ddd(9)", "method 'ddd'")
checkmessage("local a,b,c; (function () a = b+1.1 end)()", "upvalue 'b'")
assert(not doit"local aaa={bbb={ddd=next}}; aaa.bbb:ddd(nil)")

-- upvalues being indexed do not go to the stack
checkmessage("local a,b,cc; (function () a = cc[1] end)()", "upvalue 'cc'")
checkmessage("local a,b,cc; (function () a.x = 1 end)()", "upvalue 'a'")

checkmessage("local _ENV = {x={}}; a = a + 1", "global 'a'")

checkmessage("b=1; local aaa={}; x=aaa+b", "local 'aaa'")
checkmessage("aaa={}; x=3.3/aaa", "global 'aaa'")
checkmessage("aaa=2; b=nil;x=aaa*b", "global 'b'")
checkmessage("aaa={}; x=-aaa", "global 'aaa'")

-- short circuit
checkmessage("a=1; local a,bbbb=2,3; a = math.sin(1) and bbbb(3)",
       "local 'bbbb'")
checkmessage("a=1; local a,bbbb=2,3; a = bbbb(1) or a(3)", "local 'bbbb'")
checkmessage("local a,b,c,f = 1,1,1; f((a and b) or c)", "local 'f'")
checkmessage("local a,b,c = 1,1,1; ((a and b) or c)()", "call a number value")
assert(not string.find(doit"aaa={}; x=(aaa or aaa)+(aaa and aaa)", "'aaa'"))
assert(not string.find(doit"aaa={}; (aaa or aaa)()", "'aaa'"))

checkmessage("print(print < 10)", "function with number")
checkmessage("print(print < print)", "two function values")
checkmessage("print('10' < 10)", "string with number")
checkmessage("print(10 < '23')", "number with string")

-- float->integer conversions
checkmessage("local a = 2.0^100; x = a << 2", "local a")
checkmessage("local a = 1 >> 2.0^100", "has no integer representation")
checkmessage("local a = 10.1 << 2.0^100", "has no integer representation")
checkmessage("local a = 2.0^100 & 1", "has no integer representation")
checkmessage("local a = 2.0^100 & 1e100", "has no integer representation")
checkmessage("local a = 2.0 | 1e40", "has no integer representation")
checkmessage("local a = 2e100 ~ 1", "has no integer representation")
checkmessage("string.sub('a', 2.0^100)", "has no integer representation")
checkmessage("string.rep('a', 3.3)", "has no integer representation")
checkmessage("return 6e40 & 7", "has no integer representation")
checkmessage("return 34 << 7e30", "has no integer representation")
checkmessage("return ~-3e40", "has no integer representation")
checkmessage("return ~-3.009", "has no integer representation")
checkmessage("return 3.009 & 1", "has no integer representation")
checkmessage("return 34 >> {}", "table value")
checkmessage("a = 24 // 0", "divide by zero")
checkmessage("a = 1 % 0", "'n%0'")


-- type error for an object which is neither in an upvalue nor a register.
-- The following code will try to index the value 10 that is stored in
-- the metatable, without moving it to a register.
checkmessage("local a = setmetatable({}, {__index = 10}).x",
             "attempt to index a number value")


-- numeric for loops
checkmessage("for i = {}, 10 do end", "table")
checkmessage("for i = io.stdin, 10 do end", "FILE")
checkmessage("for i = {}, 10 do end", "initial value")
checkmessage("for i = 1, 'x', 10 do end", "string")
checkmessage("for i = 1, {}, 10 do end", "limit")
checkmessage("for i = 1, {} do end", "limit")
checkmessage("for i = 1, 10, print do end", "step")
checkmessage("for i = 1, 10, print do end", "function")

-- passing light userdata instead of full userdata
_G.D = debug
checkmessage([[
  -- create light udata
  local x = D.upvalueid(function () return debug end, 1)
  D.setuservalue(x, {})
]], "light userdata")
_G.D = nil

do   -- named objects (field '__name')
  checkmessage("math.sin(io.input())", "(number expected, got FILE*)")
  _G.XX = setmetatable({}, {__name = "My Type"})
  assert(string.find(tostring(XX), "^My Type"))
  checkmessage("io.input(XX)", "(FILE* expected, got My Type)")
  checkmessage("return XX + 1", "on a My Type value")
  checkmessage("return ~io.stdin", "on a FILE* value")
  checkmessage("return XX < XX", "two My Type values")
  checkmessage("return {} < XX", "table with My Type")
  checkmessage("return XX < io.stdin", "My Type with FILE*")
  _G.XX = nil
end

-- global functions
checkmessage("(io.write or print){}", "io.write")
checkmessage("(collectgarbage or print){}", "collectgarbage")

-- errors in functions without debug info
do
  local f = function (a) return a + 1 end
  f = assert(load(string.dump(f, true)))
  assert(f(3) == 4)
  checkerr("^%?:%-1:", f, {})

  -- code with a move to a local var ('OP_MOV A B' with A<B)
  f = function () local a; a = {}; return a + 2 end
  -- no debug info (so that 'a' is unknown)
  f = assert(load(string.dump(f, true)))
  -- symbolic execution should not get lost
  checkerr("^%?:%-1:.*table value", f)
end


-- tests for field accesses after RK limit
local t = {}
for i = 1, 1000 do
  t[i] = "a = x" .. i
end
local s = table.concat(t, "; ")
t = nil
checkmessage(s.."; a = bbb + 1", "global 'bbb'")
checkmessage("local _ENV=_ENV;"..s.."; a = bbb + 1", "global 'bbb'")
checkmessage(s.."; local t = {}; a = t.bbb + 1", "field 'bbb'")
checkmessage(s.."; local t = {}; t:bbb()", "method 'bbb'")

checkmessage([[aaa=9
repeat until 3==3
local x=math.sin(math.cos(3))
if math.sin(1) == x then return math.sin(1) end   -- tail call
local a,b = 1, {
  {x='a'..'b'..'c', y='b', z=x},
  {1,2,3,4,5} or 3+3<=3+3,
  3+1>3+1,
  {d = x and aaa[x or y]}}
]], "global 'aaa'")

checkmessage([[
local x,y = {},1
if math.sin(1) == 0 then return 3 end    -- return
x.a()]], "field 'a'")

checkmessage([[
prefix = nil
insert = nil
while 1 do
  local a
  if nil then break end
  insert(prefix, a)
end]], "global 'insert'")

checkmessage([[  -- tail call
  return math.sin("a")
]], "'sin'")

checkmessage([[collectgarbage("nooption")]], "invalid option")

checkmessage([[x = print .. "a"]], "concatenate")
checkmessage([[x = "a" .. false]], "concatenate")
checkmessage([[x = {} .. 2]], "concatenate")

checkmessage("getmetatable(io.stdin).__gc()", "no value")

checkmessage([[
local Var
local function main()
  NoSuchName (function() Var=0 end)
end
main()
]], "global 'NoSuchName'")
print'+'

a = {}; setmetatable(a, {__index = string})
checkmessage("a:sub()", "bad self")
checkmessage("string.sub('a', {})", "#2")
checkmessage("('a'):sub{}", "#1")

checkmessage("table.sort({1,2,3}, table.sort)", "'table.sort'")
checkmessage("string.gsub('s', 's', setmetatable)", "'setmetatable'")

-- tests for errors in coroutines

local function f (n)
  local c = coroutine.create(f)
  local a,b = coroutine.resume(c)
  return b
end
assert(string.find(f(), "C stack overflow"))

checkmessage("coroutine.yield()", "outside a coroutine")

f = coroutine.wrap(function () table.sort({1,2,3}, coroutine.yield) end)
checkerr("yield across", f)


-- testing size of 'source' info; size of buffer for that info is
-- LUA_IDSIZE, declared as 60 in luaconf. Get one position for '\0'.
idsize = 60 - 1
local function checksize (source)
  -- syntax error
  local _, msg = load("x", source)
  msg = string.match(msg, "^([^:]*):")   -- get source (1st part before ':')
  assert(msg:len() <= idsize)
end

for i = 60 - 10, 60 + 10 do   -- check border cases around 60
  checksize("@" .. string.rep("x", i))   -- file names
  checksize(string.rep("x", i - 10))     -- string sources
  checksize("=" .. string.rep("x", i))   -- exact sources
end


-- testing line error

local function lineerror (s, l)
  local err,msg = pcall(load(s))
  local line = tonumber(string.match(msg, ":(%d+):"))
  assert(line == l or (not line and not l))
end

lineerror("local a\n for i=1,'a' do \n print(i) \n end", 2)
lineerror("\n local a \n for k,v in 3 \n do \n print(k) \n end", 3)
lineerror("\n\n for k,v in \n 3 \n do \n print(k) \n end", 4)
lineerror("function a.x.y ()\na=a+1\nend", 1)

lineerror("a = \na\n+\n{}", 3)
lineerror("a = \n3\n+\n(\n4\n/\nprint)", 6)
lineerror("a = \nprint\n+\n(\n4\n/\n7)", 3)

lineerror("a\n=\n-\n\nprint\n;", 3)

lineerror([[
a
(
23)
]], 1)

lineerror([[
local a = {x = 13}
a
.
x
(
23
)
]], 2)

lineerror([[
local a = {x = 13}
a
.
x
(
23 + a
)
]], 6)

local p = [[
  function g() f() end
  function f(x) error('a', X) end
g()
]]
X=3;lineerror((p), 3)
X=0;lineerror((p), false)
X=1;lineerror((p), 2)
X=2;lineerror((p), 1)


lineerror([[
local b = false
if not b then
  error 'test'
end]], 3)

lineerror([[
local b = false
if not b then
  if not b then
    if not b then
      error 'test'
    end
  end
end]], 5)

do
  -- Force a negative estimate for base line. Error in instruction 2
  -- (after VARARGPREP, GETGLOBAL), with first absolute line information
  -- (forced by too many lines) in instruction 0.
  local s = string.format("%s return __A.x", string.rep("\n", 300))
  lineerror(s, 301)
end


if not _soft then
  -- several tests that exaust the Lua stack
  collectgarbage()
  print"testing stack overflow"
  C = 0
  -- get line where stack overflow will happen
  local l = debug.getinfo(1, "l").currentline + 1
  local function auxy () C=C+1; auxy() end     -- produce a stack overflow
  function y ()
    collectgarbage("stop")   -- avoid running finalizers without stack space
    auxy()
    collectgarbage("restart")
  end

  local function checkstackmessage (m)
    print("(expected stack overflow after " .. C .. " calls)")
    C = 0    -- prepare next count
    return (string.find(m, "stack overflow"))
  end
  -- repeated stack overflows (to check stack recovery)
  assert(checkstackmessage(doit('y()')))
  assert(checkstackmessage(doit('y()')))
  assert(checkstackmessage(doit('y()')))


  -- error lines in stack overflow
  local l1
  local function g(x)
    l1 = debug.getinfo(x, "l").currentline + 2
    collectgarbage("stop")   -- avoid running finalizers without stack space
    auxy()
    collectgarbage("restart")
  end
  local _, stackmsg = xpcall(g, debug.traceback, 1)
  print('+')
  local stack = {}
  for line in string.gmatch(stackmsg, "[^\n]*") do
    local curr = string.match(line, ":(%d+):")
    if curr then table.insert(stack, tonumber(curr)) end
  end
  local i=1
  while stack[i] ~= l1 do
    assert(stack[i] == l)
    i = i+1
  end
  assert(i > 15)


  -- error in error handling
  local res, msg = xpcall(error, error)
  assert(not res and type(msg) == 'string')
  print('+')

  local function f (x)
    if x==0 then error('a\n')
    else
      local aux = function () return f(x-1) end
      local a,b = xpcall(aux, aux)
      return a,b
    end
  end
  f(3)

  local function loop (x,y,z) return 1 + loop(x, y, z) end
 
  local res, msg = xpcall(loop, function (m)
    assert(string.find(m, "stack overflow"))
    checkerr("error handling", loop)
    assert(math.sin(0) == 0)
    return 15
  end)
  assert(msg == 15)

  local f = function ()
    for i = 999900, 1000000, 1 do table.unpack({}, 1, i) end
  end
  checkerr("too many results", f)

end


do
  -- non string messages
  local t = {}
  local res, msg = pcall(function () error(t) end)
  assert(not res and msg == t)

  res, msg = pcall(function () error(nil) end)
  assert(not res and msg == nil)

  local function f() error{msg='x'} end
  res, msg = xpcall(f, function (r) return {msg=r.msg..'y'} end)
  assert(msg.msg == 'xy')

  -- 'assert' with extra arguments
  res, msg = pcall(assert, false, "X", t)
  assert(not res and msg == "X")
 
  -- 'assert' with no message
  res, msg = pcall(function () assert(false) end)
  local line = string.match(msg, "%w+%.lua:(%d+): assertion failed!$")
  assert(tonumber(line) == debug.getinfo(1, "l").currentline - 2)

  -- 'assert' with non-string messages
  res, msg = pcall(assert, false, t)
  assert(not res and msg == t)

  res, msg = pcall(assert, nil, nil)
  assert(not res and msg == nil)

  -- 'assert' without arguments
  res, msg = pcall(assert)
  assert(not res and string.find(msg, "value expected"))
end

-- xpcall with arguments
a, b, c = xpcall(string.find, error, "alo", "al")
assert(a and b == 1 and c == 2)
a, b, c = xpcall(string.find, function (x) return {} end, true, "al")
assert(not a and type(b) == "table" and c == nil)


print("testing tokens in error messages")
checksyntax("syntax error", "", "error", 1)
checksyntax("1.000", "", "1.000", 1)
checksyntax("[[a]]", "", "[[a]]", 1)
checksyntax("'aa'", "", "'aa'", 1)
checksyntax("while << do end", "", "<<", 1)
checksyntax("for >> do end", "", ">>", 1)

-- test invalid non-printable char in a chunk
checksyntax("a\1a = 1", "", "<\\1>", 1)

-- test 255 as first char in a chunk
checksyntax("\255a = 1", "", "<\\255>", 1)

doit('I = load("a=9+"); a=3')
assert(a==3 and not I)
print('+')

lim = 1000
if _soft then lim = 100 end
for i=1,lim do
  doit('a = ')
  doit('a = 4+nil')
end


-- testing syntax limits

local function testrep (init, rep, close, repc, finalresult)
  local s = init .. string.rep(rep, 100) .. close .. string.rep(repc, 100)
  local res, msg = load(s)
  assert(res)   -- 100 levels is OK
  if (finalresult) then
    assert(res() == finalresult)
  end
  s = init .. string.rep(rep, 500)
  local res, msg = load(s)   -- 500 levels not ok
  assert(not res and (string.find(msg, "too many") or
                      string.find(msg, "overflow")))
end

testrep("local a; a", ",a", "= 1", ",1")    -- multiple assignment
testrep("local a; a=", "{", "0", "}")
testrep("return ", "(", "2", ")", 2)
testrep("local function a (x) return x end; return ", "a(", "2.2", ")", 2.2)
testrep("", "do ", "", " end")
testrep("", "while a do ", "", " end")
testrep("local a; ", "if a then else ", "", " end")
testrep("", "function foo () ", "", " end")
testrep("local a = ''; return ", "a..", "'a'", "", "a")
testrep("local a = 1; return ", "a^", "a", "", 1)

checkmessage("a = f(x" .. string.rep(",x", 260) .. ")", "too many registers")


-- testing other limits

-- upvalues
local lim = 127
local  s = "local function fooA ()\n  local "
for j = 1,lim do
  s = s.."a"..j..", "
end
s = s.."b,c\n"
s = s.."local function fooB ()\n  local "
for j = 1,lim do
  s = s.."b"..j..", "
end
s = s.."b\n"
s = s.."function fooC () return b+c"
local c = 1+2
for j = 1,lim do
  s = s.."+a"..j.."+b"..j
  c = c + 2
end
s = s.."\nend  end end"
local a,b = load(s)
assert(c > 255 and string.find(b, "too many upvalues") and
       string.find(b, "line 5"))

-- local variables
s = "\nfunction foo ()\n  local "
for j = 1,300 do
  s = s.."a"..j..", "
end
s = s.."b\n"
local a,b = load(s)
assert(string.find(b, "line 2") and string.find(b, "too many local variables"))

mt.__index = oldmm

print('OK')
