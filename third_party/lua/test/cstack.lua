-- $Id: test/cstack.lua $
-- See Copyright Notice in file all.lua


local tracegc = require"tracegc"

print"testing stack overflow detection"

-- Segmentation faults in these tests probably result from a C-stack
-- overflow. To avoid these errors, you should set a smaller limit for
-- the use of C stack by Lua, by changing the constant 'LUAI_MAXCCALLS'.
-- Alternatively, you can ensure a larger stack for the program.


local function checkerror (msg, f, ...)
  local s, err = pcall(f, ...)
  assert(not s and string.find(err, msg))
end

do  print("testing stack overflow in message handling")
  local count = 0
  local function loop (x, y, z)
    count = count + 1
    return 1 + loop(x, y, z)
  end
  tracegc.stop()    -- __gc should not be called with a full stack
  local res, msg = xpcall(loop, loop)
  tracegc.start()
  assert(msg == "error in error handling")
  print("final count: ", count)
end


-- bug since 2.5 (C-stack overflow in recursion inside pattern matching)
do  print("testing recursion inside pattern matching")
  local function f (size)
    local s = string.rep("a", size)
    local p = string.rep(".?", size)
    return string.match(s, p)
  end
  local m = f(80)
  assert(#m == 80)
  checkerror("too complex", f, 2000)
end


do  print("testing stack-overflow in recursive 'gsub'")
  local count = 0
  local function foo ()
    count = count + 1
    string.gsub("a", ".", foo)
  end
  checkerror("stack overflow", foo)
  print("final count: ", count)

  print("testing stack-overflow in recursive 'gsub' with metatables")
  local count = 0
  local t = setmetatable({}, {__index = foo})
  foo = function ()
    count = count + 1
    string.gsub("a", ".", t)
  end
  checkerror("stack overflow", foo)
  print("final count: ", count)
end


do   -- bug in 5.4.0
  print("testing limits in coroutines inside deep calls")
  local count = 0
  local lim = 1000
  local function stack (n)
    if n > 0 then return stack(n - 1) + 1
    else coroutine.wrap(function ()
           count = count + 1
           stack(lim)
         end)()
    end
  end

  local st, msg = xpcall(stack, function () return "ok" end, lim)
  assert(not st and msg == "ok")
  print("final count: ", count)
end


do
  print("nesting of resuming yielded coroutines")
  local count = 0

  local function body ()
    coroutine.yield()
    local f = coroutine.wrap(body)
    f();  -- start new coroutine (will stop in previous yield)
    count = count + 1
    f()   -- call it recursively
  end

  local f = coroutine.wrap(body)
  f()
  assert(not pcall(f))
  print("final count: ", count)
end


if T then
  print("testing stack recovery")
  local N = 0      -- trace number of calls
  local LIM = -1   -- will store N just before stack overflow

  -- trace stack size; after stack overflow, it should be
  -- the maximum allowed stack size.
  local stack1
  local dummy

  local function err(msg)
    assert(string.find(msg, "stack overflow"))
    local _, stacknow = T.stacklevel()
    assert(stacknow == stack1 + 200)
  end

  -- When LIM==-1, the 'if' is not executed, so this function only
  -- counts and stores the stack limits up to overflow.  Then, LIM
  -- becomes N, and then the 'if' code is run when the stack is
  -- full. Then, there is a stack overflow inside 'xpcall', after which
  -- the stack must have been restored back to its maximum normal size.
  local function f()
    dummy, stack1 = T.stacklevel()
    if N == LIM then
      xpcall(f, err)
      local _, stacknow = T.stacklevel()
      assert(stacknow == stack1)
      return
    end
    N = N + 1
    f()
  end

  local topB, sizeB   -- top and size Before overflow
  local topA, sizeA   -- top and size After overflow
  topB, sizeB = T.stacklevel()
  tracegc.stop()    -- __gc should not be called with a full stack
  xpcall(f, err)
  tracegc.start()
  topA, sizeA = T.stacklevel()
  -- sizes should be comparable
  assert(topA == topB and sizeA < sizeB * 2)
  print(string.format("maximum stack size: %d", stack1))
  LIM = N      -- will stop recursion at maximum level
  N = 0        -- to count again
  tracegc.stop()    -- __gc should not be called with a full stack
  f()
  tracegc.start()
  print"+"
end

print'OK'
