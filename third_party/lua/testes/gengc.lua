-- $Id: testes/gengc.lua $
-- See Copyright Notice in file all.lua

print('testing generational garbage collection')

local debug = require"debug"

assert(collectgarbage("isrunning"))

collectgarbage()

local oldmode = collectgarbage("generational")


-- ensure that table barrier evolves correctly
do
  local U = {}
  -- full collection makes 'U' old
  collectgarbage()
  assert(not T or T.gcage(U) == "old")

  -- U refers to a new table, so it becomes 'touched1'
  U[1] = {x = {234}}
  assert(not T or (T.gcage(U) == "touched1" and T.gcage(U[1]) == "new"))

  -- both U and the table survive one more collection
  collectgarbage("step", 0)
  assert(not T or (T.gcage(U) == "touched2" and T.gcage(U[1]) == "survival"))

  -- both U and the table survive yet another collection
  -- now everything is old
  collectgarbage("step", 0)
  assert(not T or (T.gcage(U) == "old" and T.gcage(U[1]) == "old1"))

  -- data was not corrupted
  assert(U[1].x[1] == 234)
end


do
  -- ensure that 'firstold1' is corrected when object is removed from
  -- the 'allgc' list
  local function foo () end
  local old = {10}
  collectgarbage()    -- make 'old' old
  assert(not T or T.gcage(old) == "old")
  setmetatable(old, {})    -- new table becomes OLD0 (barrier)
  assert(not T or T.gcage(getmetatable(old)) == "old0")
  collectgarbage("step", 0)   -- new table becomes OLD1 and firstold1
  assert(not T or T.gcage(getmetatable(old)) == "old1")
  setmetatable(getmetatable(old), {__gc = foo})  -- get it out of allgc list
  collectgarbage("step", 0)   -- should not seg. fault
end


do   -- bug in 5.4.0
-- When an object aged OLD1 is finalized, it is moved from the list
-- 'finobj' to the *beginning* of the list 'allgc', but that part of the
-- list was not being visited by 'markold'.
  local A = {}
  A[1] = false     -- old anchor for object

  -- obj finalizer
  local function gcf (obj)
    A[1] = obj     -- anchor object
    assert(not T or T.gcage(obj) == "old1")
    obj = nil      -- remove it from the stack
    collectgarbage("step", 0)   -- do a young collection
    print(getmetatable(A[1]).x)   -- metatable was collected
  end

  collectgarbage()   -- make A old
  local obj = {}     -- create a new object
  collectgarbage("step", 0)   -- make it a survival
  assert(not T or T.gcage(obj) == "survival")
  setmetatable(obj, {__gc = gcf, x = "+"})   -- create its metatable
  assert(not T or T.gcage(getmetatable(obj)) == "new")
  obj = nil   -- clear object
  collectgarbage("step", 0)   -- will call obj's finalizer
end


do   -- another bug in 5.4.0
  local old = {10}
  collectgarbage()   -- make 'old' old
  local co = coroutine.create(
    function ()
      local x = nil
      local f = function ()
                  return x[1]
                end
      x = coroutine.yield(f)
      coroutine.yield()
    end
  )
  local _, f = coroutine.resume(co)   -- create closure over 'x' in coroutine
  collectgarbage("step", 0)   -- make upvalue a survival
  old[1] = {"hello"}    -- 'old' go to grayagain as 'touched1'
  coroutine.resume(co, {123})     -- its value will be new
  co = nil
  collectgarbage("step", 0)   -- hit the barrier
  assert(f() == 123 and old[1][1] == "hello")
  collectgarbage("step", 0)   -- run the collector once more
  -- make sure old[1] was not collected
  assert(f() == 123 and old[1][1] == "hello")
end


do   -- bug introduced in commit 9cf3299fa
  local t = setmetatable({}, {__mode = "kv"})   -- all-weak table
  collectgarbage()   -- full collection
  assert(not T or T.gcage(t) == "old")
  t[1] = {10}
  assert(not T or (T.gcage(t) == "touched1" and T.gccolor(t) == "gray"))
  collectgarbage("step", 0)   -- minor collection
  assert(not T or (T.gcage(t) == "touched2" and T.gccolor(t) == "black"))
  collectgarbage("step", 0)   -- minor collection
  assert(not T or T.gcage(t) == "old")   -- t should be black, but it was gray
  t[1] = {10}      -- no barrier here, so t was still old
  collectgarbage("step", 0)   -- minor collection
  -- t, being old, is ignored by the collection, so it is not cleared
  assert(t[1] == nil)   -- fails with the bug
end


if T == nil then
  (Message or print)('\n >>> testC not active: \z
                             skipping some generational tests <<<\n')
  print 'OK'
  return
end


-- ensure that userdata barrier evolves correctly
do
  local U = T.newuserdata(0, 1)
  -- full collection makes 'U' old
  collectgarbage()
  assert(T.gcage(U) == "old")

  -- U refers to a new table, so it becomes 'touched1'
  debug.setuservalue(U, {x = {234}})
  assert(T.gcage(U) == "touched1" and
         T.gcage(debug.getuservalue(U)) == "new")

  -- both U and the table survive one more collection
  collectgarbage("step", 0)
  assert(T.gcage(U) == "touched2" and
         T.gcage(debug.getuservalue(U)) == "survival")

  -- both U and the table survive yet another collection
  -- now everything is old
  collectgarbage("step", 0)
  assert(T.gcage(U) == "old" and
         T.gcage(debug.getuservalue(U)) == "old1")

  -- data was not corrupted
  assert(debug.getuservalue(U).x[1] == 234)
end

-- just to make sure
assert(collectgarbage'isrunning')



-- just to make sure
assert(collectgarbage'isrunning')

collectgarbage(oldmode)

print('OK')

