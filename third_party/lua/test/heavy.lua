-- $Id: heavy.lua,v 1.7 2017/12/29 15:42:15 roberto Exp $
-- See Copyright Notice in file all.lua

local function teststring ()
  print("creating a string too long")
  do
    local a = "x"
    local st, msg = pcall(function ()
      while true do
        a = a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         .. a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         .. a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         .. a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         .. a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         .. a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         .. a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         .. a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         .. a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         .. a .. a.. a.. a.. a.. a.. a.. a.. a.. a
         print(string.format("string with %d bytes", #a))
      end
    end)
    assert(not st and
      (string.find(msg, "string length overflow") or
       string.find(msg, "not enough memory")))
    print("string length overflow with " .. #a * 100)
  end
  print('+')
end

local function loadrep (x, what)
  local p = 1<<20
  local s = string.rep(x, p)
  local count = 0
  local function f()
    count = count + p
    if count % (0x80*p) == 0 then
      io.stderr:write("(", count // 2^20, " M)")
    end
    return s
  end
  local st, msg = load(f, "=big")
  print("\nmemory: ", collectgarbage'count' * 1024)
  msg = string.match(msg, "^[^\n]+")    -- get only first line
  print(string.format("total: 0x%x %s ('%s')", count, what, msg))
  return st, msg
end


function controlstruct ()
  print("control structure too long")
  local lim = ((1 << 24) - 2) // 3
  local s = string.rep("a = a + 1\n", lim)
  s = "while true do " .. s .. "end"
  assert(load(s))
  print("ok with " .. lim .. " lines")
  lim = lim + 3
  s = string.rep("a = a + 1\n", lim)
  s = "while true do " .. s .. "end"
  local st, msg = load(s)
  assert(not st and string.find(msg, "too long"))
  print(msg)
end


function manylines ()
  print("loading chunk with too many lines")
  local st, msg = loadrep("\n", "lines")
  assert(not st and string.find(msg, "too many lines"))
  print('+')
end


function hugeid ()
  print("loading chunk with huge identifier")
  local st, msg = loadrep("a", "chars")
  assert(not st and 
    (string.find(msg, "lexical element too long") or
     string.find(msg, "not enough memory")))
  print('+')
end

function toomanyinst ()
  print("loading chunk with too many instructions")
  local st, msg = loadrep("a = 10; ", "instructions")
  print('+')
end


local function loadrepfunc (prefix, f)
  local count = -1
  local function aux ()
    count = count + 1
    if count == 0 then
      return prefix
    else
      if count % (0x100000) == 0 then
        io.stderr:write("(", count // 2^20, " M)")
      end
      return f(count)
    end
  end
  local st, msg = load(aux, "k")
  print("\nmemory: ", collectgarbage'count' * 1024)
  msg = string.match(msg, "^[^\n]+")    -- get only first line
  print("expected error: ", msg)
end


function toomanyconst ()
  print("loading function with too many constants")
  loadrepfunc("function foo () return {0,",
      function (n)
        -- convert 'n' to a string in the format [["...",]],
        -- where '...' is a kind of number in base 128
        -- (in a range that does not include either the double quote
        -- and the escape.)
        return string.char(34,
          ((n // 128^0) & 127) + 128,
          ((n // 128^1) & 127) + 128,
          ((n // 128^2) & 127) + 128,
          ((n // 128^3) & 127) + 128,
          ((n // 128^4) & 127) + 128,
          34, 44)
      end)
end


function toomanystr ()
  local a = {}
  local st, msg = pcall(function ()
    for i = 1, math.huge do
      if i % (0x100000) == 0 then
        io.stderr:write("(", i // 2^20, " M)")
      end
      a[i] = string.pack("I", i)
     end
  end)
  local size = #a
  a = collectgarbage'count'
  print("\nmemory:", a * 1024)
  print("expected error:", msg)
  print("size:", size)
end


function toomanyidx ()
  local a = {}
  local st, msg = pcall(function ()
    for i = 1, math.huge do
      if i % (0x100000) == 0 then
        io.stderr:write("(", i // 2^20, " M)")
      end
      a[i] = i
     end
  end)
  print("\nmemory: ", collectgarbage'count' * 1024)
  print("expected error: ", msg)
  print("size:", #a)
end
  


-- teststring()
-- controlstruct()
-- manylines()
-- hugeid()
-- toomanyinst()
-- toomanyconst()
-- toomanystr()
toomanyidx()

print "OK"
