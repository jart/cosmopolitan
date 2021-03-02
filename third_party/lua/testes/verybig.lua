-- $Id: testes/verybig.lua $
-- See Copyright Notice in file all.lua

print "testing RK"

-- testing opcodes with RK arguments larger than K limit
local function foo ()
  local dummy = {
     -- fill first 256 entries in table of constants
     1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
     17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
     33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
     49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
     65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
     81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
     97, 98, 99, 100, 101, 102, 103, 104,
     105, 106, 107, 108, 109, 110, 111, 112,
     113, 114, 115, 116, 117, 118, 119, 120,
     121, 122, 123, 124, 125, 126, 127, 128,
     129, 130, 131, 132, 133, 134, 135, 136,
     137, 138, 139, 140, 141, 142, 143, 144,
     145, 146, 147, 148, 149, 150, 151, 152,
     153, 154, 155, 156, 157, 158, 159, 160,
     161, 162, 163, 164, 165, 166, 167, 168,
     169, 170, 171, 172, 173, 174, 175, 176,
     177, 178, 179, 180, 181, 182, 183, 184,
     185, 186, 187, 188, 189, 190, 191, 192,
     193, 194, 195, 196, 197, 198, 199, 200,
     201, 202, 203, 204, 205, 206, 207, 208,
     209, 210, 211, 212, 213, 214, 215, 216,
     217, 218, 219, 220, 221, 222, 223, 224,
     225, 226, 227, 228, 229, 230, 231, 232,
     233, 234, 235, 236, 237, 238, 239, 240,
     241, 242, 243, 244, 245, 246, 247, 248,
     249, 250, 251, 252, 253, 254, 255, 256,
  }
  assert(24.5 + 0.6 == 25.1)
  local t = {foo = function (self, x) return x + self.x end, x = 10}
  t.t = t
  assert(t:foo(1.5) == 11.5)
  assert(t.t:foo(0.5) == 10.5)   -- bug in 5.2 alpha
  assert(24.3 == 24.3)
  assert((function () return t.x end)() == 10)
end


foo()
foo = nil

if _soft then return 10 end

print "testing large programs (>64k)"

-- template to create a very big test file
prog = [[$

local a,b

b = {$1$
  b30009 = 65534,
  b30010 = 65535,
  b30011 = 65536,
  b30012 = 65537,
  b30013 = 16777214,
  b30014 = 16777215,
  b30015 = 16777216,
  b30016 = 16777217,
  b30017 = 0x7fffff,
  b30018 = -0x7fffff,
  b30019 = 0x1ffffff,
  b30020 = -0x1ffffd,
  b30021 = -65534,
  b30022 = -65535,
  b30023 = -65536,
  b30024 = -0xffffff,
  b30025 = 15012.5,
  $2$
};

assert(b.a50008 == 25004 and b["a11"] == -5.5)
assert(b.a33007 == -16503.5 and b.a50009 == -25004.5)
assert(b["b"..30024] == -0xffffff)

function b:xxx (a,b) return a+b end
assert(b:xxx(10, 12) == 22)   -- pushself with non-constant index
b["xxx"] = undef

s = 0; n=0
for a,b in pairs(b) do s=s+b; n=n+1 end
-- with 32-bit floats, exact value of 's' depends on summation order
assert(81800000.0 < s and s < 81860000 and n == 70001)

a = nil; b = nil
print'+'

function f(x) b=x end

a = f{$3$} or 10

assert(a==10)
assert(b[1] == "a10" and b[2] == 5 and b[#b-1] == "a50009")


function xxxx (x) return b[x] end

assert(xxxx(3) == "a11")

a = nil; b=nil
xxxx = nil

return 10

]]

-- functions to fill in the $n$

local function sig (x)
  return (x % 2 == 0) and '' or '-'
end

F = {
function ()   -- $1$
  for i=10,50009 do
    io.write('a', i, ' = ', sig(i), 5+((i-10)/2), ',\n')
  end
end,

function ()   -- $2$
  for i=30026,50009 do
    io.write('b', i, ' = ', sig(i), 15013+((i-30026)/2), ',\n')
  end
end,

function ()   -- $3$
  for i=10,50009 do
    io.write('"a', i, '", ', sig(i), 5+((i-10)/2), ',\n')
  end
end,
}

file = os.tmpname()
io.output(file)
for s in string.gmatch(prog, "$([^$]+)") do
  local n = tonumber(s)
  if not n then io.write(s) else F[n]() end
end
io.close()
result = dofile(file)
assert(os.remove(file))
print'OK'
return result

