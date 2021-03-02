-- $Id: testes/coroutine.lua $
-- See Copyright Notice in file all.lua

print "testing coroutines"

local debug = require'debug'

local f

local main, ismain = coroutine.running()
assert(type(main) == "thread" and ismain)
assert(not coroutine.resume(main))
assert(not coroutine.isyieldable(main) and not coroutine.isyieldable())
assert(not pcall(coroutine.yield))


-- trivial errors
assert(not pcall(coroutine.resume, 0))
assert(not pcall(coroutine.status, 0))


-- tests for multiple yield/resume arguments

local function eqtab (t1, t2)
  assert(#t1 == #t2)
  for i = 1, #t1 do
    local v = t1[i]
    assert(t2[i] == v)
  end
end

_G.x = nil   -- declare x
function foo (a, ...)
  local x, y = coroutine.running()
  assert(x == f and y == false)
  -- next call should not corrupt coroutine (but must fail,
  -- as it attempts to resume the running coroutine)
  assert(coroutine.resume(f) == false)
  assert(coroutine.status(f) == "running")
  local arg = {...}
  assert(coroutine.isyieldable(x))
  for i=1,#arg do
    _G.x = {coroutine.yield(table.unpack(arg[i]))}
  end
  return table.unpack(a)
end

f = coroutine.create(foo)
assert(coroutine.isyieldable(f))
assert(type(f) == "thread" and coroutine.status(f) == "suspended")
assert(string.find(tostring(f), "thread"))
local s,a,b,c,d
s,a,b,c,d = coroutine.resume(f, {1,2,3}, {}, {1}, {'a', 'b', 'c'})
assert(coroutine.isyieldable(f))
assert(s and a == nil and coroutine.status(f) == "suspended")
s,a,b,c,d = coroutine.resume(f)
eqtab(_G.x, {})
assert(s and a == 1 and b == nil)
assert(coroutine.isyieldable(f))
s,a,b,c,d = coroutine.resume(f, 1, 2, 3)
eqtab(_G.x, {1, 2, 3})
assert(s and a == 'a' and b == 'b' and c == 'c' and d == nil)
s,a,b,c,d = coroutine.resume(f, "xuxu")
eqtab(_G.x, {"xuxu"})
assert(s and a == 1 and b == 2 and c == 3 and d == nil)
assert(coroutine.status(f) == "dead")
s, a = coroutine.resume(f, "xuxu")
assert(not s and string.find(a, "dead") and coroutine.status(f) == "dead")


-- yields in tail calls
local function foo (i) return coroutine.yield(i) end
f = coroutine.wrap(function ()
  for i=1,10 do
    assert(foo(i) == _G.x)
  end
  return 'a'
end)
for i=1,10 do _G.x = i; assert(f(i) == i) end
_G.x = 'xuxu'; assert(f('xuxu') == 'a')

-- recursive
function pf (n, i)
  coroutine.yield(n)
  pf(n*i, i+1)
end

f = coroutine.wrap(pf)
local s=1
for i=1,10 do
  assert(f(1, 1) == s)
  s = s*i
end

-- sieve
function gen (n)
  return coroutine.wrap(function ()
    for i=2,n do coroutine.yield(i) end
  end)
end


function filter (p, g)
  return coroutine.wrap(function ()
    while 1 do
      local n = g()
      if n == nil then return end
      if math.fmod(n, p) ~= 0 then coroutine.yield(n) end
    end
  end)
end

local x = gen(80)
local a = {}
while 1 do
  local n = x()
  if n == nil then break end
  table.insert(a, n)
  x = filter(n, x)
end

assert(#a == 22 and a[#a] == 79)
x, a = nil


print("to-be-closed variables in coroutines")

local function func2close (f)
  return setmetatable({}, {__close = f})
end

do
  -- ok to close a dead coroutine
  local co = coroutine.create(print)
  assert(coroutine.resume(co, "testing 'coroutine.close'"))
  assert(coroutine.status(co) == "dead")
  local st, msg = coroutine.close(co)
  assert(st and msg == nil)

  -- cannot close the running coroutine
  local st, msg = pcall(coroutine.close, coroutine.running())
  assert(not st and string.find(msg, "running"))

  local main = coroutine.running()

  -- cannot close a "normal" coroutine
  ;(coroutine.wrap(function ()
    local st, msg = pcall(coroutine.close, main)
    assert(not st and string.find(msg, "normal"))
  end))()

  -- to-be-closed variables in coroutines
  local X

  -- closing a coroutine after an error
  local co = coroutine.create(error)
  local st, msg = coroutine.resume(co, 100)
  assert(not st and msg == 100)
  st, msg = coroutine.close(co)
  assert(not st and msg == 100)

  co = coroutine.create(function ()
    local x <close> = func2close(function (self, err)
      assert(err == nil); X = false
    end)
    X = true
    coroutine.yield()
  end)
  coroutine.resume(co)
  assert(X)
  assert(coroutine.close(co))
  assert(not X and coroutine.status(co) == "dead")

  -- error closing a coroutine
  local x = 0
  co = coroutine.create(function()
    local y <close> = func2close(function (self,err)
      assert(err == 111)
      x = 200
      error(200)
    end)
    local x <close> = func2close(function (self, err)
      assert(err == nil); error(111)
    end)
    coroutine.yield()
  end)
  coroutine.resume(co)
  assert(x == 0)
  local st, msg = coroutine.close(co)
  assert(st == false and coroutine.status(co) == "dead" and msg == 200)
  assert(x == 200)
end

do
  -- <close> versus pcall in coroutines
  local X = false
  local Y = false
  function foo ()
    local x <close> = func2close(function (self, err)
      Y = debug.getinfo(2)
      X = err
    end)
    error(43)
  end
  co = coroutine.create(function () return pcall(foo) end)
  local st1, st2, err = coroutine.resume(co)
  assert(st1 and not st2 and err == 43)
  assert(X == 43 and Y.name == "pcall")

  -- recovering from errors in __close metamethods
  local track = {}

  local function h (o)
    local hv <close> = o
    return 1
  end

  local function foo ()
    local x <close> = func2close(function(_,msg)
      track[#track + 1] = msg or false
      error(20)
    end)
    local y <close> = func2close(function(_,msg)
      track[#track + 1] = msg or false
      return 1000
    end)
    local z <close> = func2close(function(_,msg)
      track[#track + 1] = msg or false
      error(10)
    end)
    coroutine.yield(1)
    h(func2close(function(_,msg)
        track[#track + 1] = msg or false
        error(2)
      end))
  end

  local co = coroutine.create(pcall)

  local st, res = coroutine.resume(co, foo)    -- call 'foo' protected
  assert(st and res == 1)   -- yield 1
  local st, res1, res2 = coroutine.resume(co)   -- continue
  assert(coroutine.status(co) == "dead")
  assert(st and not res1 and res2 == 20)   -- last error (20)
  assert(track[1] == false and track[2] == 2 and track[3] == 10 and
         track[4] == 10)
end


-- yielding across C boundaries

co = coroutine.wrap(function()
       assert(not pcall(table.sort,{1,2,3}, coroutine.yield))
       assert(coroutine.isyieldable())
       coroutine.yield(20)
       return 30
     end)

assert(co() == 20)
assert(co() == 30)


local f = function (s, i) return coroutine.yield(i) end

local f1 = coroutine.wrap(function ()
             return xpcall(pcall, function (...) return ... end,
               function ()
                 local s = 0
                 for i in f, nil, 1 do pcall(function () s = s + i end) end
                 error({s})
               end)
           end)

f1()
for i = 1, 10 do assert(f1(i) == i) end
local r1, r2, v = f1(nil)
assert(r1 and not r2 and v[1] ==  (10 + 1)*10/2)


function f (a, b) a = coroutine.yield(a);  error{a + b} end
function g(x) return x[1]*2 end

co = coroutine.wrap(function ()
       coroutine.yield(xpcall(f, g, 10, 20))
     end)

assert(co() == 10)
r, msg = co(100)
assert(not r and msg == 240)


-- unyieldable C call
do
  local function f (c)
          assert(not coroutine.isyieldable())
          return c .. c
        end

  local co = coroutine.wrap(function (c)
               assert(coroutine.isyieldable())
               local s = string.gsub("a", ".", f)
               return s
             end)
  assert(co() == "aa")
end



do   -- testing single trace of coroutines
  local X
  local co = coroutine.create(function ()
    coroutine.yield(10)
    return 20;
  end)
  local trace = {}
  local function dotrace (event)
    trace[#trace + 1] = event
  end
  debug.sethook(co, dotrace, "clr")
  repeat until not coroutine.resume(co)
  local correcttrace = {"call", "line", "call", "return", "line", "return"}
  assert(#trace == #correcttrace)
  for k, v in pairs(trace) do
    assert(v == correcttrace[k])
  end
end

-- errors in coroutines
function foo ()
  assert(debug.getinfo(1).currentline == debug.getinfo(foo).linedefined + 1)
  assert(debug.getinfo(2).currentline == debug.getinfo(goo).linedefined)
  coroutine.yield(3)
  error(foo)
end

function goo() foo() end
x = coroutine.wrap(goo)
assert(x() == 3)
local a,b = pcall(x)
assert(not a and b == foo)

x = coroutine.create(goo)
a,b = coroutine.resume(x)
assert(a and b == 3)
a,b = coroutine.resume(x)
assert(not a and b == foo and coroutine.status(x) == "dead")
a,b = coroutine.resume(x)
assert(not a and string.find(b, "dead") and coroutine.status(x) == "dead")


-- co-routines x for loop
function all (a, n, k)
  if k == 0 then coroutine.yield(a)
  else
    for i=1,n do
      a[k] = i
      all(a, n, k-1)
    end
  end
end

local a = 0
for t in coroutine.wrap(function () all({}, 5, 4) end) do
  a = a+1
end
assert(a == 5^4)


-- access to locals of collected corroutines
local C = {}; setmetatable(C, {__mode = "kv"})
local x = coroutine.wrap (function ()
            local a = 10
            local function f () a = a+10; return a end
            while true do
              a = a+1
              coroutine.yield(f)
            end
          end)

C[1] = x;

local f = x()
assert(f() == 21 and x()() == 32 and x() == f)
x = nil
collectgarbage()
assert(C[1] == undef)
assert(f() == 43 and f() == 53)


-- old bug: attempt to resume itself

function co_func (current_co)
  assert(coroutine.running() == current_co)
  assert(coroutine.resume(current_co) == false)
  coroutine.yield(10, 20)
  assert(coroutine.resume(current_co) == false)
  coroutine.yield(23)
  return 10
end

local co = coroutine.create(co_func)
local a,b,c = coroutine.resume(co, co)
assert(a == true and b == 10 and c == 20)
a,b = coroutine.resume(co, co)
assert(a == true and b == 23)
a,b = coroutine.resume(co, co)
assert(a == true and b == 10)
assert(coroutine.resume(co, co) == false)
assert(coroutine.resume(co, co) == false)


-- other old bug when attempting to resume itself
-- (trigger C-code assertions)
do
  local A = coroutine.running()
  local B = coroutine.create(function() return coroutine.resume(A) end)
  local st, res = coroutine.resume(B)
  assert(st == true and res == false)

  local X = false
  A = coroutine.wrap(function()
    local _ <close> = setmetatable({}, {__close = function () X = true end})
    return pcall(A, 1)
  end)
  st, res = A()
  assert(not st and string.find(res, "non%-suspended") and X == true)
end


-- attempt to resume 'normal' coroutine
local co1, co2
co1 = coroutine.create(function () return co2() end)
co2 = coroutine.wrap(function ()
        assert(coroutine.status(co1) == 'normal')
        assert(not coroutine.resume(co1))
        coroutine.yield(3)
      end)

a,b = coroutine.resume(co1)
assert(a and b == 3)
assert(coroutine.status(co1) == 'dead')

-- infinite recursion of coroutines
a = function(a) coroutine.wrap(a)(a) end
assert(not pcall(a, a))
a = nil


-- access to locals of erroneous coroutines
local x = coroutine.create (function ()
            local a = 10
            _G.f = function () a=a+1; return a end
            error('x')
          end)

assert(not coroutine.resume(x))
-- overwrite previous position of local `a'
assert(not coroutine.resume(x, 1, 1, 1, 1, 1, 1, 1))
assert(_G.f() == 11)
assert(_G.f() == 12)


if not T then
  (Message or print)
      ('\n >>> testC not active: skipping coroutine API tests <<<\n')
else
  print "testing yields inside hooks"

  local turn
  
  function fact (t, x)
    assert(turn == t)
    if x == 0 then return 1
    else return x*fact(t, x-1)
    end
  end

  local A, B = 0, 0

  local x = coroutine.create(function ()
    T.sethook("yield 0", "", 2)
    A = fact("A", 6)
  end)

  local y = coroutine.create(function ()
    T.sethook("yield 0", "", 3)
    B = fact("B", 7)
  end)

  while A==0 or B==0 do    -- A ~= 0 when 'x' finishes (similar for 'B','y')
    if A==0 then turn = "A"; assert(T.resume(x)) end
    if B==0 then turn = "B"; assert(T.resume(y)) end

    -- check that traceback works correctly after yields inside hooks
    debug.traceback(x)
    debug.traceback(y)
  end

  assert(B // A == 7)    -- fact(7) // fact(6)

  do   -- hooks vs. multiple values
    local done
    local function test (n)
      done = false
      return coroutine.wrap(function ()
        local a = {}
        for i = 1, n do a[i] = i end
        -- 'pushint' just to perturb the stack
        T.sethook("pushint 10; yield 0", "", 1)   -- yield at each op.
        local a1 = {table.unpack(a)}   -- must keep top between ops.
        assert(#a1 == n)
        for i = 1, n do assert(a[i] == i) end
        done = true
      end)
    end
    -- arguments to the coroutine are just to perturb its stack
    local co = test(0); while not done do co(30) end
    co = test(1); while not done do co(20, 10) end
    co = test(3); while not done do co() end
    co = test(100); while not done do co() end
  end

  local line = debug.getinfo(1, "l").currentline + 2    -- get line number
  local function foo ()
    local x = 10    --<< this line is 'line'
    x = x + 10
    _G.XX = x
  end

  -- testing yields in line hook
  local co = coroutine.wrap(function ()
    T.sethook("setglobal X; yield 0", "l", 0); foo(); return 10 end)

  _G.XX = nil;
  _G.X = nil; co(); assert(_G.X == line)
  _G.X = nil; co(); assert(_G.X == line + 1)
  _G.X = nil; co(); assert(_G.X == line + 2 and _G.XX == nil)
  _G.X = nil; co(); assert(_G.X == line + 3 and _G.XX == 20)
  assert(co() == 10)

  -- testing yields in count hook
  co = coroutine.wrap(function ()
    T.sethook("yield 0", "", 1); foo(); return 10 end)

  _G.XX = nil;
  local c = 0
  repeat c = c + 1; local a = co() until a == 10
  assert(_G.XX == 20 and c >= 5)

  co = coroutine.wrap(function ()
    T.sethook("yield 0", "", 2); foo(); return 10 end)

  _G.XX = nil;
  local c = 0
  repeat c = c + 1; local a = co() until a == 10
  assert(_G.XX == 20 and c >= 5)
  _G.X = nil; _G.XX = nil

  do
    -- testing debug library on a coroutine suspended inside a hook
    -- (bug in 5.2/5.3)
    c = coroutine.create(function (a, ...)
      T.sethook("yield 0", "l")   -- will yield on next two lines
      assert(a == 10)
      return ...
    end)

    assert(coroutine.resume(c, 1, 2, 3))   -- start coroutine
    local n,v = debug.getlocal(c, 0, 1)    -- check its local
    assert(n == "a" and v == 1)
    assert(debug.setlocal(c, 0, 1, 10))     -- test 'setlocal'
    local t = debug.getinfo(c, 0)        -- test 'getinfo'
    assert(t.currentline == t.linedefined + 1)
    assert(not debug.getinfo(c, 1))      -- no other level
    assert(coroutine.resume(c))          -- run next line
    v = {coroutine.resume(c)}         -- finish coroutine
    assert(v[1] == true and v[2] == 2 and v[3] == 3 and v[4] == undef)
    assert(not coroutine.resume(c))
  end

  do
    -- testing debug library on last function in a suspended coroutine
    -- (bug in 5.2/5.3)
    local c = coroutine.create(function () T.testC("yield 1", 10, 20) end)
    local a, b = coroutine.resume(c)
    assert(a and b == 20)
    assert(debug.getinfo(c, 0).linedefined == -1)
    a, b = debug.getlocal(c, 0, 2)
    assert(b == 10)
  end


  print "testing coroutine API"
  
  -- reusing a thread
  assert(T.testC([[
    newthread      # create thread
    pushvalue 2    # push body
    pushstring 'a a a'  # push argument
    xmove 0 3 2   # move values to new thread
    resume -1, 1    # call it first time
    pushstatus
    xmove 3 0 0   # move results back to stack
    setglobal X    # result
    setglobal Y    # status
    pushvalue 2     # push body (to call it again)
    pushstring 'b b b'
    xmove 0 3 2
    resume -1, 1    # call it again
    pushstatus
    xmove 3 0 0
    return 1        # return result
  ]], function (...) return ... end) == 'b b b')

  assert(X == 'a a a' and Y == 'OK')


  -- resuming running coroutine
  C = coroutine.create(function ()
        return T.testC([[
                 pushnum 10;
                 pushnum 20;
                 resume -3 2;
                 pushstatus
                 gettop;
                 return 3]], C)
      end)
  local a, b, c, d = coroutine.resume(C)
  assert(a == true and string.find(b, "non%-suspended") and
         c == "ERRRUN" and d == 4)

  a, b, c, d = T.testC([[
    rawgeti R 1    # get main thread
    pushnum 10;
    pushnum 20;
    resume -3 2;
    pushstatus
    gettop;
    return 4]])
  assert(a == coroutine.running() and string.find(b, "non%-suspended") and
         c == "ERRRUN" and d == 4)


  -- using a main thread as a coroutine  (dubious use!)
  local state = T.newstate()

  -- check that yielddable is working correctly
  assert(T.testC(state, "newthread; isyieldable -1; remove 1; return 1"))

  -- main thread is not yieldable
  assert(not T.testC(state, "rawgeti R 1; isyieldable -1; remove 1; return 1"))

  T.testC(state, "settop 0")

  T.loadlib(state)

  assert(T.doremote(state, [[
    coroutine = require'coroutine';
    X = function (x) coroutine.yield(x, 'BB'); return 'CC' end;
    return 'ok']]))

  t = table.pack(T.testC(state, [[
    rawgeti R 1     # get main thread
    pushstring 'XX'
    getglobal X    # get function for body
    pushstring AA      # arg
    resume 1 1      # 'resume' shadows previous stack!
    gettop
    setglobal T    # top
    setglobal B    # second yielded value
    setglobal A    # fist yielded value
    rawgeti R 1     # get main thread
    pushnum 5       # arg (noise)
    resume 1 1      # after coroutine ends, previous stack is back
    pushstatus
    return *
  ]]))
  assert(t.n == 4 and t[2] == 'XX' and t[3] == 'CC' and t[4] == 'OK')
  assert(T.doremote(state, "return T") == '2')
  assert(T.doremote(state, "return A") == 'AA')
  assert(T.doremote(state, "return B") == 'BB')

  T.closestate(state)

  print'+'

end


-- leaving a pending coroutine open
_X = coroutine.wrap(function ()
      local a = 10
      local x = function () a = a+1 end
      coroutine.yield()
    end)

_X()


if not _soft then
  -- bug (stack overflow)
  local j = 2^9
  local lim = 1000000    -- (C stack limit; assume 32-bit machine)
  local t = {lim - 10, lim - 5, lim - 1, lim, lim + 1}
  for i = 1, #t do
    local j = t[i]
    co = coroutine.create(function()
           local t = {}
           for i = 1, j do t[i] = i end
           return table.unpack(t)
         end)
    local r, msg = coroutine.resume(co)
    assert(not r)
  end
  co = nil
end


assert(coroutine.running() == main)

print"+"


print"testing yields inside metamethods"

local function val(x)
  if type(x) == "table" then return x.x else return x end
end

local mt = {
  __eq = function(a,b) coroutine.yield(nil, "eq"); return val(a) == val(b) end,
  __lt = function(a,b) coroutine.yield(nil, "lt"); return val(a) < val(b) end,
  __le = function(a,b) coroutine.yield(nil, "le"); return a - b <= 0 end,
  __add = function(a,b) coroutine.yield(nil, "add");
                        return val(a) + val(b) end,
  __sub = function(a,b) coroutine.yield(nil, "sub"); return val(a) - val(b) end,
  __mul = function(a,b) coroutine.yield(nil, "mul"); return val(a) * val(b) end,
  __div = function(a,b) coroutine.yield(nil, "div"); return val(a) / val(b) end,
  __idiv = function(a,b) coroutine.yield(nil, "idiv");
                         return val(a) // val(b) end,
  __pow = function(a,b) coroutine.yield(nil, "pow"); return val(a) ^ val(b) end,
  __mod = function(a,b) coroutine.yield(nil, "mod"); return val(a) % val(b) end,
  __unm = function(a,b) coroutine.yield(nil, "unm"); return -val(a) end,
  __bnot = function(a,b) coroutine.yield(nil, "bnot"); return ~val(a) end,
  __shl = function(a,b) coroutine.yield(nil, "shl");
                        return val(a) << val(b) end,
  __shr = function(a,b) coroutine.yield(nil, "shr");
                        return val(a) >> val(b) end,
  __band = function(a,b)
             coroutine.yield(nil, "band")
             return val(a) & val(b)
           end,
  __bor = function(a,b) coroutine.yield(nil, "bor");
                        return val(a) | val(b) end,
  __bxor = function(a,b) coroutine.yield(nil, "bxor");
                         return val(a) ~ val(b) end,

  __concat = function(a,b)
               coroutine.yield(nil, "concat");
               return val(a) .. val(b)
             end,
  __index = function (t,k) coroutine.yield(nil, "idx"); return t.k[k] end,
  __newindex = function (t,k,v) coroutine.yield(nil, "nidx"); t.k[k] = v end,
}


local function new (x)
  return setmetatable({x = x, k = {}}, mt)
end


local a = new(10)
local b = new(12)
local c = new"hello"

local function run (f, t)
  local i = 1
  local c = coroutine.wrap(f)
  while true do
    local res, stat = c()
    if res then assert(t[i] == undef); return res, t end
    assert(stat == t[i])
    i = i + 1
  end
end


assert(run(function () if (a>=b) then return '>=' else return '<' end end,
       {"le", "sub"}) == "<")
assert(run(function () if (a<=b) then return '<=' else return '>' end end,
       {"le", "sub"}) == "<=")
assert(run(function () if (a==b) then return '==' else return '~=' end end,
       {"eq"}) == "~=")

assert(run(function () return a & b + a end, {"add", "band"}) == 2)

assert(run(function () return 1 + a end, {"add"}) == 11)
assert(run(function () return a - 25 end, {"sub"}) == -15)
assert(run(function () return 2 * a end, {"mul"}) == 20)
assert(run(function () return a ^ 2 end, {"pow"}) == 100)
assert(run(function () return a / 2 end, {"div"}) == 5)
assert(run(function () return a % 6 end, {"mod"}) == 4)
assert(run(function () return a // 3 end, {"idiv"}) == 3)

assert(run(function () return a + b end, {"add"}) == 22)
assert(run(function () return a - b end, {"sub"}) == -2)
assert(run(function () return a * b end, {"mul"}) == 120)
assert(run(function () return a ^ b end, {"pow"}) == 10^12)
assert(run(function () return a / b end, {"div"}) == 10/12)
assert(run(function () return a % b end, {"mod"}) == 10)
assert(run(function () return a // b end, {"idiv"}) == 0)

-- repeat tests with larger constants (to use 'K' opcodes)
local a1000 = new(1000)

assert(run(function () return a1000 + 1000 end, {"add"}) == 2000)
assert(run(function () return a1000 - 25000 end, {"sub"}) == -24000)
assert(run(function () return 2000 * a end, {"mul"}) == 20000)
assert(run(function () return a1000 / 1000 end, {"div"}) == 1)
assert(run(function () return a1000 % 600 end, {"mod"}) == 400)
assert(run(function () return a1000 // 500 end, {"idiv"}) == 2)



assert(run(function () return a % b end, {"mod"}) == 10)

assert(run(function () return ~a & b end, {"bnot", "band"}) == ~10 & 12)
assert(run(function () return a | b end, {"bor"}) == 10 | 12)
assert(run(function () return a ~ b end, {"bxor"}) == 10 ~ 12)
assert(run(function () return a << b end, {"shl"}) == 10 << 12)
assert(run(function () return a >> b end, {"shr"}) == 10 >> 12)

assert(run(function () return 10 & b end, {"band"}) == 10 & 12)
assert(run(function () return a | 2 end, {"bor"}) == 10 | 2)
assert(run(function () return a ~ 2 end, {"bxor"}) == 10 ~ 2)
assert(run(function () return a >> 2 end, {"shr"}) == 10 >> 2)
assert(run(function () return 1 >> a end, {"shr"}) == 1 >> 10)
assert(run(function () return a << 2 end, {"shl"}) == 10 << 2)
assert(run(function () return 1 << a end, {"shl"}) == 1 << 10)
assert(run(function () return 2 ~ a end, {"bxor"}) == 2 ~ 10)


assert(run(function () return a..b end, {"concat"}) == "1012")

assert(run(function() return a .. b .. c .. a end,
       {"concat", "concat", "concat"}) == "1012hello10")

assert(run(function() return "a" .. "b" .. a .. "c" .. c .. b .. "x" end,
       {"concat", "concat", "concat"}) == "ab10chello12x")


do   -- a few more tests for comparison operators
  local mt1 = {
    __le = function (a,b)
      coroutine.yield(10)
      return (val(a) <= val(b))
    end,
    __lt = function (a,b)
      coroutine.yield(10)
      return val(a) < val(b)
    end,
  }
  local mt2 = { __lt = mt1.__lt, __le = mt1.__le }

  local function run (f)
    local co = coroutine.wrap(f)
    local res
    repeat
      res = co()
    until res ~= 10
    return res
  end
  
  local function test ()
    local a1 = setmetatable({x=1}, mt1)
    local a2 = setmetatable({x=2}, mt2)
    assert(a1 < a2)
    assert(a1 <= a2)
    assert(1 < a2)
    assert(1 <= a2)
    assert(2 > a1)
    assert(2 >= a2)
    return true
  end
  
  run(test)

end

assert(run(function ()
             a.BB = print
             return a.BB
           end, {"nidx", "idx"}) == print)

-- getuptable & setuptable
do local _ENV = _ENV
  f = function () AAA = BBB + 1; return AAA end
end
g = new(10); g.k.BBB = 10;
debug.setupvalue(f, 1, g)
assert(run(f, {"idx", "nidx", "idx"}) == 11)
assert(g.k.AAA == 11)

print"+"

print"testing yields inside 'for' iterators"

local f = function (s, i)
      if i%2 == 0 then coroutine.yield(nil, "for") end
      if i < s then return i + 1 end
    end

assert(run(function ()
             local s = 0
             for i in f, 4, 0 do s = s + i end
             return s
           end, {"for", "for", "for"}) == 10)



-- tests for coroutine API
if T==nil then
  (Message or print)('\n >>> testC not active: skipping coroutine API tests <<<\n')
  print "OK"; return
end

print('testing coroutine API')

local function apico (...)
  local x = {...}
  return coroutine.wrap(function ()
    return T.testC(table.unpack(x))
  end)
end

local a = {apico(
[[
  pushstring errorcode
  pcallk 1 0 2;
  invalid command (should not arrive here)
]],
[[return *]],
"stackmark",
error
)()}
assert(#a == 4 and
       a[3] == "stackmark" and
       a[4] == "errorcode" and
       _G.status == "ERRRUN" and
       _G.ctx == 2)       -- 'ctx' to pcallk

local co = apico(
  "pushvalue 2; pushnum 10; pcallk 1 2 3; invalid command;",
  coroutine.yield,
  "getglobal status; getglobal ctx; pushvalue 2; pushstring a; pcallk 1 0 4; invalid command",
  "getglobal status; getglobal ctx; return *")

assert(co() == 10)
assert(co(20, 30) == 'a')
a = {co()}
assert(#a == 10 and
       a[2] == coroutine.yield and
       a[5] == 20 and a[6] == 30 and
       a[7] == "YIELD" and a[8] == 3 and
       a[9] == "YIELD" and a[10] == 4)
assert(not pcall(co))   -- coroutine is dead now


f = T.makeCfunc("pushnum 3; pushnum 5; yield 1;")
co = coroutine.wrap(function ()
  assert(f() == 23); assert(f() == 23); return 10
end)
assert(co(23,16) == 5)
assert(co(23,16) == 5)
assert(co(23,16) == 10)


-- testing coroutines with C bodies
f = T.makeCfunc([[
        pushnum 102
	yieldk	1 U2
	cannot be here!
]],
[[      # continuation
	pushvalue U3   # accessing upvalues inside a continuation
        pushvalue U4
	return *
]], 23, "huu")

x = coroutine.wrap(f)
assert(x() == 102)
eqtab({x()}, {23, "huu"})


f = T.makeCfunc[[pushstring 'a'; pushnum 102; yield 2; ]]

a, b, c, d = T.testC([[newthread; pushvalue 2; xmove 0 3 1; resume 3 0;
                       pushstatus; xmove 3 0 0;  resume 3 0; pushstatus;
                       return 4; ]], f)

assert(a == 'YIELD' and b == 'a' and c == 102 and d == 'OK')


-- testing chain of suspendable C calls

local count = 3   -- number of levels

f = T.makeCfunc([[
  remove 1;             # remove argument
  pushvalue U3;         # get selection function
  call 0 1;             # call it  (result is 'f' or 'yield')
  pushstring hello      # single argument for selected function
  pushupvalueindex 2;   # index of continuation program
  callk 1 -1 .;		# call selected function
  errorerror		# should never arrive here
]],
[[
  # continuation program
  pushnum 34	# return value
  return *     # return all results
]],
function ()     -- selection function
  count = count - 1
  if count == 0 then return coroutine.yield
  else return f
  end
end
)

co = coroutine.wrap(function () return f(nil) end)
assert(co() == "hello")   -- argument to 'yield'
a = {co()}
-- three '34's (one from each pending C call)
assert(#a == 3 and a[1] == a[2] and a[2] == a[3] and a[3] == 34)


-- testing yields with continuations

co = coroutine.wrap(function (...) return
       T.testC([[ # initial function
          yieldk 1 2
          cannot be here!
       ]],
       [[  # 1st continuation
         yieldk 0 3 
         cannot be here!
       ]],
       [[  # 2nd continuation
         yieldk 0 4 
         cannot be here!
       ]],
       [[  # 3th continuation
          pushvalue 6   # function which is last arg. to 'testC' here
          pushnum 10; pushnum 20;
          pcall 2 0 0   # call should throw an error and return to next line
          pop 1		# remove error message
          pushvalue 6
          getglobal status; getglobal ctx
          pcallk 2 2 5  # call should throw an error and jump to continuation
          cannot be here!
       ]],
       [[  # 4th (and last) continuation
         return *
       ]],
       -- function called by 3th continuation
       function (a,b) x=a; y=b; error("errmsg") end,
       ...
)
end)

local a = {co(3,4,6)}
assert(a[1] == 6 and a[2] == undef)
a = {co()}; assert(a[1] == undef and _G.status == "YIELD" and _G.ctx == 2)
a = {co()}; assert(a[1] == undef and _G.status == "YIELD" and _G.ctx == 3)
a = {co(7,8)};
-- original arguments
assert(type(a[1]) == 'string' and type(a[2]) == 'string' and
     type(a[3]) == 'string' and type(a[4]) == 'string' and
     type(a[5]) == 'string' and type(a[6]) == 'function')
-- arguments left from fist resume
assert(a[7] == 3 and a[8] == 4)
-- arguments to last resume
assert(a[9] == 7 and a[10] == 8)
-- error message and nothing more
assert(a[11]:find("errmsg") and #a == 11)
-- check arguments to pcallk
assert(x == "YIELD" and y == 4)

assert(not pcall(co))   -- coroutine should be dead


-- bug in nCcalls
local co = coroutine.wrap(function ()
  local a = {pcall(pcall,pcall,pcall,pcall,pcall,pcall,pcall,error,"hi")}
  return pcall(assert, table.unpack(a))
end)

local a = {co()}
assert(a[10] == "hi")

print'OK'
