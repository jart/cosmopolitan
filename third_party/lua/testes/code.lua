-- $Id: testes/code.lua $
-- See Copyright Notice in file all.lua

if T==nil then
  (Message or print)('\n >>> testC not active: skipping opcode tests <<<\n')
  return
end
print "testing code generation and optimizations"

-- to test constant propagation
local k0aux <const> = 0
local k0 <const> = k0aux
local k1 <const> = 1
local k3 <const> = 3
local k6 <const> = k3 + (k3 << k0)
local kFF0 <const> = 0xFF0
local k3_78 <const> = 3.78
local x, k3_78_4 <const> = 10, k3_78 / 4
assert(x == 10)

local kx <const> = "x"

local kTrue <const> = true
local kFalse <const> = false

local kNil <const> = nil

-- this code gave an error for the code checker
do
  local function f (a)
  for k,v,w in a do end
  end
end


-- testing reuse in constant table
local function checkKlist (func, list)
  local k = T.listk(func)
  assert(#k == #list)
  for i = 1, #k do
    assert(k[i] == list[i] and math.type(k[i]) == math.type(list[i]))
  end
end

local function foo ()
  local a
  a = k3;
  a = 0; a = 0.0; a = -7 + 7
  a = k3_78/4; a = k3_78_4
  a = -k3_78/4; a = k3_78/4; a = -3.78/4
  a = -3.79/4; a = 0.0; a = -0;
  a = k3; a = 3.0; a = 3; a = 3.0
end

checkKlist(foo, {3.78/4, -3.78/4, -3.79/4})


foo = function (f, a)
        f(100 * 1000)
        f(100.0 * 1000)
        f(-100 * 1000)
        f(-100 * 1000.0)
        f(100000)
        f(100000.0)
        f(-100000)
        f(-100000.0)
      end

checkKlist(foo, {100000, 100000.0, -100000, -100000.0})


-- testing opcodes

-- check that 'f' opcodes match '...'
function check (f, ...)
  local arg = {...}
  local c = T.listcode(f)
  for i=1, #arg do
    local opcode = string.match(c[i], "%u%w+")
    -- print(arg[i], opcode)
    assert(arg[i] == opcode)
  end
  assert(c[#arg+2] == undef)
end


-- check that 'f' opcodes match '...' and that 'f(p) == r'.
function checkR (f, p, r, ...)
  local r1 = f(p)
  assert(r == r1 and math.type(r) == math.type(r1))
  check(f, ...)
end


-- check that 'a' and 'b' has the same opcodes
function checkequal (a, b)
  a = T.listcode(a)
  b = T.listcode(b)
  assert(#a == #b)
  for i = 1, #a do
    a[i] = string.gsub(a[i], '%b()', '')   -- remove line number
    b[i] = string.gsub(b[i], '%b()', '')   -- remove line number
    assert(a[i] == b[i])
  end
end


-- some basic instructions
check(function ()   -- function does not create upvalues
  (function () end){f()}
end, 'CLOSURE', 'NEWTABLE', 'EXTRAARG', 'GETTABUP', 'CALL',
     'SETLIST', 'CALL', 'RETURN0')

check(function (x)   -- function creates upvalues
  (function () return x end){f()}
end, 'CLOSURE', 'NEWTABLE', 'EXTRAARG', 'GETTABUP', 'CALL',
     'SETLIST', 'CALL', 'RETURN')


-- sequence of LOADNILs
check(function ()
  local kNil <const> = nil
  local a,b,c
  local d; local e;
  local f,g,h;
  d = nil; d=nil; b=nil; a=kNil; c=nil;
end, 'LOADNIL', 'RETURN0')

check(function ()
  local a,b,c,d = 1,1,1,1
  d=nil;c=nil;b=nil;a=nil
end, 'LOADI', 'LOADI', 'LOADI', 'LOADI', 'LOADNIL', 'RETURN0')

do
  local a,b,c,d = 1,1,1,1
  d=nil;c=nil;b=nil;a=nil
  assert(a == nil and b == nil and c == nil and d == nil)
end


-- single return
check (function (a,b,c) return a end, 'RETURN1')


-- infinite loops
check(function () while kTrue do local a = -1 end end,
'LOADI', 'JMP', 'RETURN0')

check(function () while 1 do local a = -1 end end,
'LOADI', 'JMP', 'RETURN0')

check(function () repeat local x = 1 until true end,
'LOADI', 'RETURN0')


-- concat optimization
check(function (a,b,c,d) return a..b..c..d end,
  'MOVE', 'MOVE', 'MOVE', 'MOVE', 'CONCAT', 'RETURN1')

-- not
check(function () return not not nil end, 'LOADFALSE', 'RETURN1')
check(function () return not not kFalse end, 'LOADFALSE', 'RETURN1')
check(function () return not not true end, 'LOADTRUE', 'RETURN1')
check(function () return not not k3 end, 'LOADTRUE', 'RETURN1')

-- direct access to locals
check(function ()
  local a,b,c,d
  a = b*a
  c.x, a[b] = -((a + d/b - a[b]) ^ a.x), b
end,
  'LOADNIL',
  'MUL', 'MMBIN',
  'DIV', 'MMBIN', 'ADD', 'MMBIN', 'GETTABLE', 'SUB', 'MMBIN',
  'GETFIELD', 'POW', 'MMBIN', 'UNM', 'SETTABLE', 'SETFIELD', 'RETURN0')


-- direct access to constants
check(function ()
  local a,b
  local c = kNil
  a[kx] = 3.2
  a.x = b
  a[b] = 'x'
end,
  'LOADNIL', 'SETFIELD', 'SETFIELD', 'SETTABLE', 'RETURN0')

-- "get/set table" with numeric indices
check(function (a)
  local k255 <const> = 255
  a[1] = a[100]
  a[k255] = a[256]
  a[256] = 5
end,
  'GETI', 'SETI',
  'LOADI', 'GETTABLE', 'SETI',
  'LOADI', 'SETTABLE',  'RETURN0')

check(function ()
  local a,b
  a = a - a
  b = a/a
  b = 5-4
end,
  'LOADNIL', 'SUB', 'MMBIN', 'DIV', 'MMBIN', 'LOADI', 'RETURN0')

check(function ()
  local a,b
  a[kTrue] = false
end,
  'LOADNIL', 'LOADTRUE', 'SETTABLE', 'RETURN0')


-- equalities
checkR(function (a) if a == 1 then return 2 end end, 1, 2,
  'EQI', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if -4.0 == a then return 2 end end, -4, 2,
  'EQI', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if a == "hi" then return 2 end end, 10, nil,
  'EQK', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if a == 10000 then return 2 end end, 1, nil,
  'EQK', 'JMP', 'LOADI', 'RETURN1')   -- number too large

checkR(function (a) if -10000 == a then return 2 end end, -10000, 2,
  'EQK', 'JMP', 'LOADI', 'RETURN1')   -- number too large

-- comparisons

checkR(function (a) if -10 <= a then return 2 end end, -10, 2,
  'GEI', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if 128.0 > a then return 2 end end, 129, nil,
  'LTI', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if -127.0 < a then return 2 end end, -127, nil,
  'GTI', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if 10 < a then return 2 end end, 11, 2,
  'GTI', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if 129 < a then return 2 end end, 130, 2,
  'LOADI', 'LT', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if a >= 23.0 then return 2 end end, 25, 2,
  'GEI', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if a >= 23.1 then return 2 end end, 0, nil,
  'LOADK', 'LE', 'JMP', 'LOADI', 'RETURN1')

checkR(function (a) if a > 2300.0 then return 2 end end, 0, nil,
  'LOADF', 'LT', 'JMP', 'LOADI', 'RETURN1')


-- constant folding
local function checkK (func, val)
  check(func, 'LOADK', 'RETURN1')
  checkKlist(func, {val})
  assert(func() == val)
end

local function checkI (func, val)
  check(func, 'LOADI', 'RETURN1')
  checkKlist(func, {})
  assert(func() == val)
end

local function checkF (func, val)
  check(func, 'LOADF', 'RETURN1')
  checkKlist(func, {})
  assert(func() == val)
end

checkF(function () return 0.0 end, 0.0)
checkI(function () return k0 end, 0)
checkI(function () return -k0//1 end, 0)
checkK(function () return 3^-1 end, 1/3)
checkK(function () return (1 + 1)^(50 + 50) end, 2^100)
checkK(function () return (-2)^(31 - 2) end, -0x20000000 + 0.0)
checkF(function () return (-k3^0 + 5) // 3.0 end, 1.0)
checkI(function () return -k3 % 5 end, 2)
checkF(function () return -((2.0^8 + -(-1)) % 8)/2 * 4 - 3 end, -5.0)
checkF(function () return -((2^8 + -(-1)) % 8)//2 * 4 - 3 end, -7.0)
checkI(function () return 0xF0.0 | 0xCC.0 ~ 0xAA & 0xFD end, 0xF4)
checkI(function () return ~(~kFF0 | kFF0) end, 0)
checkI(function () return ~~-1024.0 end, -1024)
checkI(function () return ((100 << k6) << -4) >> 2 end, 100)

-- borders around MAXARG_sBx ((((1 << 17) - 1) >> 1) == 65535)
local a = 17; local sbx = ((1 << a) - 1) >> 1   -- avoid folding
local border <const> = 65535
checkI(function () return border end, sbx)
checkI(function () return -border end, -sbx)
checkI(function () return border + 1 end, sbx + 1)
checkK(function () return border + 2 end, sbx + 2)
checkK(function () return -(border + 1) end, -(sbx + 1))

local border <const> = 65535.0
checkF(function () return border end, sbx + 0.0)
checkF(function () return -border end, -sbx + 0.0)
checkF(function () return border + 1 end, (sbx + 1.0))
checkK(function () return border + 2 end, (sbx + 2.0))
checkK(function () return -(border + 1) end, -(sbx + 1.0))


-- immediate operands
checkR(function (x) return x + k1 end, 10, 11, 'ADDI', 'MMBINI', 'RETURN1')
checkR(function (x) return x - 127 end, 10, -117, 'ADDI', 'MMBINI', 'RETURN1')
checkR(function (x) return 128 + x end, 0.0, 128.0,
         'ADDI', 'MMBINI', 'RETURN1')
checkR(function (x) return x * -127 end, -1.0, 127.0,
         'MULK', 'MMBINK', 'RETURN1')
checkR(function (x) return 20 * x end, 2, 40, 'MULK', 'MMBINK', 'RETURN1')
checkR(function (x) return x ^ -2 end, 2, 0.25, 'POWK', 'MMBINK', 'RETURN1')
checkR(function (x) return x / 40 end, 40, 1.0, 'DIVK', 'MMBINK', 'RETURN1')
checkR(function (x) return x // 1 end, 10.0, 10.0,
         'IDIVK', 'MMBINK', 'RETURN1')
checkR(function (x) return x % (100 - 10) end, 91, 1,
         'MODK', 'MMBINK', 'RETURN1')
checkR(function (x) return k1 << x end, 3, 8, 'SHLI', 'MMBINI', 'RETURN1')
checkR(function (x) return x << 127 end, 10, 0, 'SHRI', 'MMBINI', 'RETURN1')
checkR(function (x) return x << -127 end, 10, 0, 'SHRI', 'MMBINI', 'RETURN1')
checkR(function (x) return x >> 128 end, 8, 0, 'SHRI', 'MMBINI', 'RETURN1')
checkR(function (x) return x >> -127 end, 8, 0, 'SHRI', 'MMBINI', 'RETURN1')
checkR(function (x) return x & 1 end, 9, 1, 'BANDK', 'MMBINK', 'RETURN1')
checkR(function (x) return 10 | x end, 1, 11, 'BORK', 'MMBINK', 'RETURN1')
checkR(function (x) return -10 ~ x end, -1, 9, 'BXORK', 'MMBINK', 'RETURN1')

-- K operands in arithmetic operations
checkR(function (x) return x + 0.0 end, 1, 1.0, 'ADDK', 'MMBINK', 'RETURN1')
--  check(function (x) return 128 + x end, 'ADDK', 'MMBINK', 'RETURN1')
checkR(function (x) return x * -10000 end, 2, -20000,
         'MULK', 'MMBINK', 'RETURN1')
--  check(function (x) return 20 * x end, 'MULK', 'MMBINK', 'RETURN1')
checkR(function (x) return x ^ 0.5 end, 4, 2.0, 'POWK', 'MMBINK', 'RETURN1')
checkR(function (x) return x / 2.0 end, 4, 2.0, 'DIVK', 'MMBINK', 'RETURN1')
checkR(function (x) return x // 10000 end, 10000, 1,
         'IDIVK', 'MMBINK', 'RETURN1')
checkR(function (x) return x % (100.0 - 10) end, 91, 1.0,
         'MODK', 'MMBINK', 'RETURN1')

-- no foldings (and immediate operands)
check(function () return -0.0 end, 'LOADF', 'UNM', 'RETURN1')
check(function () return k3/0 end, 'LOADI', 'DIVK', 'MMBINK', 'RETURN1')
check(function () return 0%0 end, 'LOADI', 'MODK', 'MMBINK', 'RETURN1')
check(function () return -4//0 end, 'LOADI', 'IDIVK', 'MMBINK', 'RETURN1')
check(function (x) return x >> 2.0 end, 'LOADF', 'SHR', 'MMBIN', 'RETURN1')
check(function (x) return x << 128 end, 'LOADI', 'SHL', 'MMBIN', 'RETURN1')
check(function (x) return x & 2.0 end, 'LOADF', 'BAND', 'MMBIN', 'RETURN1')

-- basic 'for' loops
check(function () for i = -10, 10.5 do end end,
'LOADI', 'LOADK', 'LOADI', 'FORPREP', 'FORLOOP', 'RETURN0')
check(function () for i = 0xfffffff, 10.0, 1 do end end,
'LOADK', 'LOADF', 'LOADI', 'FORPREP', 'FORLOOP', 'RETURN0')

-- bug in constant folding for 5.1
check(function () return -nil end, 'LOADNIL', 'UNM', 'RETURN1')


check(function ()
  local a,b,c
  b[c], a = c, b
  b[a], a = c, b
  a, b = c, a
  a = a
end,
  'LOADNIL',
  'MOVE', 'MOVE', 'SETTABLE',
  'MOVE', 'MOVE', 'MOVE', 'SETTABLE',
  'MOVE', 'MOVE', 'MOVE',
  -- no code for a = a
  'RETURN0')


-- x == nil , x ~= nil
-- checkequal(function (b) if (a==nil) then a=1 end; if a~=nil then a=1 end end,
--            function () if (a==9) then a=1 end; if a~=9 then a=1 end end)

-- check(function () if a==nil then a='a' end end,
-- 'GETTABUP', 'EQ', 'JMP', 'SETTABUP', 'RETURN')

do   -- tests for table access in upvalues
  local t
  check(function () t[kx] = t.y end, 'GETTABUP', 'SETTABUP')
  check(function (a) t[a()] = t[a()] end,
  'MOVE', 'CALL', 'GETUPVAL', 'MOVE', 'CALL',
  'GETUPVAL', 'GETTABLE', 'SETTABLE')
end

-- de morgan
checkequal(function () local a; if not (a or b) then b=a end end,
           function () local a; if (not a and not b) then b=a end end)

checkequal(function (l) local a; return 0 <= a and a <= l end,
           function (l) local a; return not (not(a >= 0) or not(a <= l)) end)


-- if-break optimizations
check(function (a, b)
        while a do
          if b then break else a = a + 1 end
        end
      end,
'TEST', 'JMP', 'TEST', 'JMP', 'ADDI', 'MMBINI', 'JMP', 'RETURN0')

checkequal(function () return 6 or true or nil end,
           function () return k6 or kTrue or kNil end)

checkequal(function () return 6 and true or nil end,
           function () return k6 and kTrue or kNil end)


do   -- string constants
  local k0 <const> = "00000000000000000000000000000000000000000000000000"
  local function f1 ()
    local k <const> = k0
    return function ()
             return function () return k end
           end
  end

  local f2 = f1()
  local f3 = f2()
  assert(f3() == k0)
  checkK(f3, k0)
  -- string is not needed by other functions
  assert(T.listk(f1)[1] == nil)
  assert(T.listk(f2)[1] == nil)
end

print 'OK'

