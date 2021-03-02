-- $Id: testes/events.lua $
-- See Copyright Notice in file all.lua

print('testing metatables')

local debug = require'debug'

X = 20; B = 30

_ENV = setmetatable({}, {__index=_G})

collectgarbage()

X = X+10
assert(X == 30 and _G.X == 20)
B = false
assert(B == false)
_ENV["B"] = undef
assert(B == 30)

assert(getmetatable{} == nil)
assert(getmetatable(4) == nil)
assert(getmetatable(nil) == nil)
a={name = "NAME"}; setmetatable(a, {__metatable = "xuxu",
                    __tostring=function(x) return x.name end})
assert(getmetatable(a) == "xuxu")
assert(tostring(a) == "NAME")
-- cannot change a protected metatable
assert(pcall(setmetatable, a, {}) == false)
a.name = "gororoba"
assert(tostring(a) == "gororoba")

local a, t = {10,20,30; x="10", y="20"}, {}
assert(setmetatable(a,t) == a)
assert(getmetatable(a) == t)
assert(setmetatable(a,nil) == a)
assert(getmetatable(a) == nil)
assert(setmetatable(a,t) == a)


function f (t, i, e)
  assert(not e)
  local p = rawget(t, "parent")
  return (p and p[i]+3), "dummy return"
end

t.__index = f

a.parent = {z=25, x=12, [4] = 24}
assert(a[1] == 10 and a.z == 28 and a[4] == 27 and a.x == "10")

collectgarbage()

a = setmetatable({}, t)
function f(t, i, v) rawset(t, i, v-3) end
setmetatable(t, t)   -- causes a bug in 5.1 !
t.__newindex = f
a[1] = 30; a.x = "101"; a[5] = 200
assert(a[1] == 27 and a.x == 98 and a[5] == 197)

do    -- bug in Lua 5.3.2
  local mt = {}
  mt.__newindex = mt
  local t = setmetatable({}, mt)
  t[1] = 10     -- will segfault on some machines
  assert(mt[1] == 10)
end


local c = {}
a = setmetatable({}, t)
t.__newindex = c
t.__index = c
a[1] = 10; a[2] = 20; a[3] = 90;
for i = 4, 20 do a[i] = i * 10 end
assert(a[1] == 10 and a[2] == 20 and a[3] == 90)
for i = 4, 20 do assert(a[i] == i * 10) end
assert(next(a) == nil)


do
  local a;
  a = setmetatable({}, {__index = setmetatable({},
                     {__index = setmetatable({},
                     {__index = function (_,n) return a[n-3]+4, "lixo" end})})})
  a[0] = 20
  for i=0,10 do
    assert(a[i*3] == 20 + i*4)
  end
end


do  -- newindex
  local foi
  local a = {}
  for i=1,10 do a[i] = 0; a['a'..i] = 0; end
  setmetatable(a, {__newindex = function (t,k,v) foi=true; rawset(t,k,v) end})
  foi = false; a[1]=0; assert(not foi)
  foi = false; a['a1']=0; assert(not foi)
  foi = false; a['a11']=0; assert(foi)
  foi = false; a[11]=0; assert(foi)
  foi = false; a[1]=undef; assert(not foi)
  a[1] = undef
  foi = false; a[1]=nil; assert(foi)
end


setmetatable(t, nil)
function f (t, ...) return t, {...} end
t.__call = f

do
  local x,y = a(table.unpack{'a', 1})
  assert(x==a and y[1]=='a' and y[2]==1 and y[3]==undef)
  x,y = a()
  assert(x==a and y[1]==undef)
end


local b = setmetatable({}, t)
setmetatable(b,t)

function f(op)
  return function (...) cap = {[0] = op, ...} ; return (...) end
end
t.__add = f("add")
t.__sub = f("sub")
t.__mul = f("mul")
t.__div = f("div")
t.__idiv = f("idiv")
t.__mod = f("mod")
t.__unm = f("unm")
t.__pow = f("pow")
t.__len = f("len")
t.__band = f("band")
t.__bor = f("bor")
t.__bxor = f("bxor")
t.__shl = f("shl")
t.__shr = f("shr")
t.__bnot = f("bnot")
t.__lt = f("lt")
t.__le = f("le")


local function checkcap (t)
  assert(#cap + 1 == #t)
  for i = 1, #t do
    assert(cap[i - 1] == t[i])
    assert(math.type(cap[i - 1]) == math.type(t[i]))
  end
end

-- Some tests are done inside small anonymous functions to ensure
-- that constants go to constant table even in debug compilation,
-- when the constant table is very small.
assert(b+5 == b); checkcap{"add", b, 5}
assert(5.2 + b == 5.2); checkcap{"add", 5.2, b}
assert(b+'5' == b); checkcap{"add", b, '5'}
assert(5+b == 5); checkcap{"add", 5, b}
assert('5'+b == '5'); checkcap{"add", '5', b}
b=b-3; assert(getmetatable(b) == t); checkcap{"sub", b, 3}
assert(5-a == 5); checkcap{"sub", 5, a}
assert('5'-a == '5'); checkcap{"sub", '5', a}
assert(a*a == a); checkcap{"mul", a, a}
assert(a/0 == a); checkcap{"div", a, 0}
assert(a/0.0 == a); checkcap{"div", a, 0.0}
assert(a%2 == a); checkcap{"mod", a, 2}
assert(a // (1/0) == a); checkcap{"idiv", a, 1/0}
;(function () assert(a & "hi" == a) end)(); checkcap{"band", a, "hi"}
;(function () assert(10 & a  == 10) end)(); checkcap{"band", 10, a}
;(function () assert(a | 10  == a) end)(); checkcap{"bor", a, 10}
assert(a | "hi" == a); checkcap{"bor", a, "hi"}
assert("hi" ~ a == "hi"); checkcap{"bxor", "hi", a}
;(function () assert(10 ~ a == 10) end)(); checkcap{"bxor", 10, a}
assert(-a == a); checkcap{"unm", a, a}
assert(a^4.0 == a); checkcap{"pow", a, 4.0}
assert(a^'4' == a); checkcap{"pow", a, '4'}
assert(4^a == 4); checkcap{"pow", 4, a}
assert('4'^a == '4'); checkcap{"pow", '4', a}
assert(#a == a); checkcap{"len", a, a}
assert(~a == a); checkcap{"bnot", a, a}
assert(a << 3 == a); checkcap{"shl", a, 3}
assert(1.5 >> a == 1.5); checkcap{"shr", 1.5, a}

-- for comparison operators, all results are true
assert(5.0 > a); checkcap{"lt", a, 5.0}
assert(a >= 10); checkcap{"le", 10, a}
assert(a <= -10.0); checkcap{"le", a, -10.0}
assert(a < -10); checkcap{"lt", a, -10}


-- test for rawlen
t = setmetatable({1,2,3}, {__len = function () return 10 end})
assert(#t == 10 and rawlen(t) == 3)
assert(rawlen"abc" == 3)
assert(not pcall(rawlen, io.stdin))
assert(not pcall(rawlen, 34))
assert(not pcall(rawlen))

-- rawlen for long strings
assert(rawlen(string.rep('a', 1000)) == 1000)


t = {}
t.__lt = function (a,b,c)
  collectgarbage()
  assert(c == nil)
  if type(a) == 'table' then a = a.x end
  if type(b) == 'table' then b = b.x end
 return a<b, "dummy"
end

t.__le = function (a,b,c)
  assert(c == nil)
  if type(a) == 'table' then a = a.x end
  if type(b) == 'table' then b = b.x end
 return a<=b, "dummy"
end

t.__eq = function (a,b,c)
  assert(c == nil)
  if type(a) == 'table' then a = a.x end
  if type(b) == 'table' then b = b.x end
 return a == b, "dummy"
end

function Op(x) return setmetatable({x=x}, t) end

local function test (a, b, c)
  assert(not(Op(1)<Op(1)) and (Op(1)<Op(2)) and not(Op(2)<Op(1)))
  assert(not(1 < Op(1)) and (Op(1) < 2) and not(2 < Op(1)))
  assert(not(Op('a')<Op('a')) and (Op('a')<Op('b')) and not(Op('b')<Op('a')))
  assert(not('a' < Op('a')) and (Op('a') < 'b') and not(Op('b') < Op('a')))
  assert((Op(1)<=Op(1)) and (Op(1)<=Op(2)) and not(Op(2)<=Op(1)))
  assert((Op('a')<=Op('a')) and (Op('a')<=Op('b')) and not(Op('b')<=Op('a')))
  assert(not(Op(1)>Op(1)) and not(Op(1)>Op(2)) and (Op(2)>Op(1)))
  assert(not(Op('a')>Op('a')) and not(Op('a')>Op('b')) and (Op('b')>Op('a')))
  assert((Op(1)>=Op(1)) and not(Op(1)>=Op(2)) and (Op(2)>=Op(1)))
  assert((1 >= Op(1)) and not(1 >= Op(2)) and (Op(2) >= 1))
  assert((Op('a')>=Op('a')) and not(Op('a')>=Op('b')) and (Op('b')>=Op('a')))
  assert(('a' >= Op('a')) and not(Op('a') >= 'b') and (Op('b') >= Op('a')))
  assert(Op(1) == Op(1) and Op(1) ~= Op(2))
  assert(Op('a') == Op('a') and Op('a') ~= Op('b'))
  assert(a == a and a ~= b)
  assert(Op(3) == c)
end

test(Op(1), Op(2), Op(3))


-- test `partial order'

local function rawSet(x)
  local y = {}
  for _,k in pairs(x) do y[k] = 1 end
  return y
end

local function Set(x)
  return setmetatable(rawSet(x), t)
end

t.__lt = function (a,b)
  for k in pairs(a) do
    if not b[k] then return false end
    b[k] = undef
  end
  return next(b) ~= nil
end

t.__le = function (a,b)
  for k in pairs(a) do
    if not b[k] then return false end
  end
  return true
end

assert(Set{1,2,3} < Set{1,2,3,4})
assert(not(Set{1,2,3,4} < Set{1,2,3,4}))
assert((Set{1,2,3,4} <= Set{1,2,3,4}))
assert((Set{1,2,3,4} >= Set{1,2,3,4}))
assert(not (Set{1,3} <= Set{3,5}))
assert(not(Set{1,3} <= Set{3,5}))
assert(not(Set{1,3} >= Set{3,5}))


t.__eq = function (a,b)
  for k in pairs(a) do
    if not b[k] then return false end
    b[k] = undef
  end
  return next(b) == nil
end

local s = Set{1,3,5}
assert(s == Set{3,5,1})
assert(not rawequal(s, Set{3,5,1}))
assert(rawequal(s, s))
assert(Set{1,3,5,1} == rawSet{3,5,1})
assert(rawSet{1,3,5,1} == Set{3,5,1})
assert(Set{1,3,5} ~= Set{3,5,1,6})

-- '__eq' is not used for table accesses
t[Set{1,3,5}] = 1
assert(t[Set{1,3,5}] == undef)


do   -- test invalidating flags
  local mt = {__eq = true}
  local a = setmetatable({10}, mt)
  local b = setmetatable({10}, mt)
  mt.__eq = nil
  assert(a ~= b)   -- no metamethod
  mt.__eq = function (x,y) return x[1] == y[1] end
  assert(a == b)   -- must use metamethod now
end


if not T then
  (Message or print)('\n >>> testC not active: skipping tests for \z
userdata <<<\n')
else
  local u1 = T.newuserdata(0, 1)
  local u2 = T.newuserdata(0, 1)
  local u3 = T.newuserdata(0, 1)
  assert(u1 ~= u2 and u1 ~= u3)
  debug.setuservalue(u1, 1);
  debug.setuservalue(u2, 2);
  debug.setuservalue(u3, 1);
  debug.setmetatable(u1, {__eq = function (a, b)
    return debug.getuservalue(a) == debug.getuservalue(b)
  end})
  debug.setmetatable(u2, {__eq = function (a, b)
    return true
  end})
  assert(u1 == u3 and u3 == u1 and u1 ~= u2)
  assert(u2 == u1 and u2 == u3 and u3 == u2)
  assert(u2 ~= {})   -- different types cannot be equal
  assert(rawequal(u1, u1) and not rawequal(u1, u3))

  local mirror = {}
  debug.setmetatable(u3, {__index = mirror, __newindex = mirror})
  for i = 1, 10 do u3[i] = i end
  for i = 1, 10 do assert(u3[i] == i) end
end


t.__concat = function (a,b,c)
  assert(c == nil)
  if type(a) == 'table' then a = a.val end
  if type(b) == 'table' then b = b.val end
  if A then return a..b
  else
    return setmetatable({val=a..b}, t)
  end
end

c = {val="c"}; setmetatable(c, t)
d = {val="d"}; setmetatable(d, t)

A = true
assert(c..d == 'cd')
assert(0 .."a".."b"..c..d.."e".."f"..(5+3).."g" == "0abcdef8g")

A = false
assert((c..d..c..d).val == 'cdcd')
x = c..d
assert(getmetatable(x) == t and x.val == 'cd')
x = 0 .."a".."b"..c..d.."e".."f".."g"
assert(x.val == "0abcdefg")


-- concat metamethod x numbers (bug in 5.1.1)
c = {}
local x
setmetatable(c, {__concat = function (a,b)
  assert(type(a) == "number" and b == c or type(b) == "number" and a == c)
  return c
end})
assert(c..5 == c and 5 .. c == c)
assert(4 .. c .. 5 == c and 4 .. 5 .. 6 .. 7 .. c == c)


-- test comparison compatibilities
local t1, t2, c, d
t1 = {};  c = {}; setmetatable(c, t1)
d = {}
t1.__eq = function () return true end
t1.__lt = function () return true end
t1.__le = function () return false end
setmetatable(d, t1)
assert(c == d and c < d and not(d <= c))
t2 = {}
t2.__eq = t1.__eq
t2.__lt = t1.__lt
setmetatable(d, t2)
assert(c == d and c < d and not(d <= c))



-- test for several levels of calls
local i
local tt = {
  __call = function (t, ...)
    i = i+1
    if t.f then return t.f(...)
    else return {...}
    end
  end
}

local a = setmetatable({}, tt)
local b = setmetatable({f=a}, tt)
local c = setmetatable({f=b}, tt)

i = 0
x = c(3,4,5)
assert(i == 3 and x[1] == 3 and x[3] == 5)


assert(_G.X == 20)

print'+'

local _g = _G
_ENV = setmetatable({}, {__index=function (_,k) return _g[k] end})


a = {}
rawset(a, "x", 1, 2, 3)
assert(a.x == 1 and rawget(a, "x", 3) == 1)

print '+'

-- testing metatables for basic types
mt = {__index = function (a,b) return a+b end,
      __len = function (x) return math.floor(x) end}
debug.setmetatable(10, mt)
assert(getmetatable(-2) == mt)
assert((10)[3] == 13)
assert((10)["3"] == 13)
assert(#3.45 == 3)
debug.setmetatable(23, nil)
assert(getmetatable(-2) == nil)

debug.setmetatable(true, mt)
assert(getmetatable(false) == mt)
mt.__index = function (a,b) return a or b end
assert((true)[false] == true)
assert((false)[false] == false)
debug.setmetatable(false, nil)
assert(getmetatable(true) == nil)

debug.setmetatable(nil, mt)
assert(getmetatable(nil) == mt)
mt.__add = function (a,b) return (a or 1) + (b or 2) end
assert(10 + nil == 12)
assert(nil + 23 == 24)
assert(nil + nil == 3)
debug.setmetatable(nil, nil)
assert(getmetatable(nil) == nil)

debug.setmetatable(nil, {})


-- loops in delegation
a = {}; setmetatable(a, a); a.__index = a; a.__newindex = a
assert(not pcall(function (a,b) return a[b] end, a, 10))
assert(not pcall(function (a,b,c) a[b] = c end, a, 10, true))

-- bug in 5.1
T, K, V = nil
grandparent = {}
grandparent.__newindex = function(t,k,v) T=t; K=k; V=v end

parent = {}
parent.__newindex = parent
setmetatable(parent, grandparent)

child = setmetatable({}, parent)
child.foo = 10      --> CRASH (on some machines)
assert(T == parent and K == "foo" and V == 10)

print 'OK'

return 12


