#!../lua
-- $Id: testes/all.lua $
-- See Copyright Notice at the end of this file


local version = "Lua 5.4"
if _VERSION ~= version then
  io.stderr:write("This test suite is for ", version,
                  ", not for ", _VERSION, "\nExiting tests")
  return
end


_G.ARG = arg   -- save arg for other tests


-- next variables control the execution of some tests
-- true means no test (so an undefined variable does not skip a test)
-- defaults are for Linux; test everything.
-- Make true to avoid long or memory consuming tests
_soft = rawget(_G, "_soft") or false
-- Make true to avoid non-portable tests
_port = rawget(_G, "_port") or false
-- Make true to avoid messages about tests not performed
_nomsg = rawget(_G, "_nomsg") or false


local usertests = rawget(_G, "_U")

if usertests then
  -- tests for sissies ;)  Avoid problems
  _soft = true
  _port = true
  _nomsg = true
end

-- tests should require debug when needed
debug = nil


if usertests then
  T = nil    -- no "internal" tests for user tests
else
  T = rawget(_G, "T")  -- avoid problems with 'strict' module
end


--[=[
  example of a long [comment],
  [[spanning several [lines]]]

]=]

print("\n\tStarting Tests")

do
  -- set random seed
  local random_x, random_y = math.randomseed()
  print(string.format("random seeds: %d, %d", random_x, random_y))
end

print("current path:\n****" .. package.path .. "****\n")


local initclock = os.clock()
local lastclock = initclock
local walltime = os.time()

local collectgarbage = collectgarbage

do   -- (

-- track messages for tests not performed
local msgs = {}
function Message (m)
  if not _nomsg then
    print(m)
    msgs[#msgs+1] = string.sub(m, 3, -3)
  end
end

assert(os.setlocale"C")

local T,print,format,write,assert,type,unpack,floor =
      T,print,string.format,io.write,assert,type,table.unpack,math.floor

-- use K for 1000 and M for 1000000 (not 2^10 -- 2^20)
local function F (m)
  local function round (m)
    m = m + 0.04999
    return format("%.1f", m)      -- keep one decimal digit
  end
  if m < 1000 then return m
  else
    m = m / 1000
    if m < 1000 then return round(m).."K"
    else
      return round(m/1000).."M"
    end
  end
end

local Cstacklevel

local showmem
if not T then
  local max = 0
  showmem = function ()
    local m = collectgarbage("count") * 1024
    max = (m > max) and m or max
    print(format("    ---- total memory: %s, max memory: %s ----\n",
          F(m), F(max)))
  end
  Cstacklevel = function () return 0 end   -- no info about stack level
else
  showmem = function ()
    T.checkmemory()
    local total, numblocks, maxmem = T.totalmem()
    local count = collectgarbage("count")
    print(format(
      "\n    ---- total memory: %s (%.0fK), max use: %s,  blocks: %d\n",
      F(total), count, F(maxmem), numblocks))
    print(format("\t(strings:  %d, tables: %d, functions: %d, "..
                 "\n\tudata: %d, threads: %d)",
                 T.totalmem"string", T.totalmem"table", T.totalmem"function",
                 T.totalmem"userdata", T.totalmem"thread"))
  end

  Cstacklevel = function ()
    local _, _, ncalls = T.stacklevel()
    return ncalls    -- number of C calls
  end
end


local Cstack = Cstacklevel()

--
-- redefine dofile to run files through dump/undump
--
local function report (n) print("\n***** FILE '"..n.."'*****") end
local olddofile = dofile
local dofile = function (n, strip)
  showmem()
  local c = os.clock()
  print(string.format("time: %g (+%g)", c - initclock, c - lastclock))
  lastclock = c
  report(n)
  local f = assert(loadfile(n))
  local b = string.dump(f, strip)
  f = assert(load(b))
  return f()
end

dofile('main.lua')

do
  local next, setmetatable, stderr = next, setmetatable, io.stderr
  -- track collections
  local mt = {}
  -- each time a table is collected, remark it for finalization
  -- on next cycle
  mt.__gc = function (o)
     stderr:write'.'    -- mark progress
     local n = setmetatable(o, mt)   -- remark it
   end
   local n = setmetatable({}, mt)    -- create object
end

report"gc.lua"
local f = assert(loadfile('gc.lua'))
f()

dofile('db.lua')
assert(dofile('calls.lua') == deep and deep)
olddofile('strings.lua')
olddofile('literals.lua')
dofile('tpack.lua')
assert(dofile('attrib.lua') == 27)
dofile('gengc.lua')
assert(dofile('locals.lua') == 5)
dofile('constructs.lua')
dofile('code.lua', true)
if not _G._soft then
  report('big.lua')
  local f = coroutine.wrap(assert(loadfile('big.lua')))
  assert(f() == 'b')
  assert(f() == 'a')
end
dofile('cstack.lua')
dofile('nextvar.lua')
dofile('pm.lua')
dofile('utf8.lua')
dofile('api.lua')
assert(dofile('events.lua') == 12)
dofile('vararg.lua')
dofile('closure.lua')
dofile('coroutine.lua')
dofile('goto.lua', true)
dofile('errors.lua')
dofile('math.lua')
dofile('sort.lua', true)
dofile('bitwise.lua')
assert(dofile('verybig.lua', true) == 10); collectgarbage()
dofile('files.lua')

if #msgs > 0 then
  local m = table.concat(msgs, "\n  ")
  warn("#tests not performed:\n  ", m, "\n")
end

print("(there should be two warnings now)")
warn("@on")
warn("#This is ", "an expected", " warning")
warn("@off")
warn("******** THIS WARNING SHOULD NOT APPEAR **********")
warn("******** THIS WARNING ALSO SHOULD NOT APPEAR **********")
warn("@on")
warn("#This is", " another one")

-- no test module should define 'debug'
assert(debug == nil)

local debug = require "debug"

print(string.format("%d-bit integers, %d-bit floats",
        string.packsize("j") * 8, string.packsize("n") * 8))

debug.sethook(function (a) assert(type(a) == 'string') end, "cr")

-- to survive outside block
_G.showmem = showmem


assert(Cstack == Cstacklevel(),
  "should be at the same C-stack level it was when started the tests")

end   --)

local _G, showmem, print, format, clock, time, difftime,
      assert, open, warn =
      _G, showmem, print, string.format, os.clock, os.time, os.difftime,
      assert, io.open, warn

-- file with time of last performed test
local fname = T and "time-debug.txt" or "time.txt"
local lasttime

if not usertests then
  -- open file with time of last performed test
  local f = io.open(fname)
  if f then
    lasttime = assert(tonumber(f:read'a'))
    f:close();
  else   -- no such file; assume it is recording time for first time
    lasttime = nil
  end
end

-- erase (almost) all globals
print('cleaning all!!!!')
for n in pairs(_G) do
  if not ({___Glob = 1, tostring = 1})[n] then
    _G[n] = undef
  end
end


collectgarbage()
collectgarbage()
collectgarbage()
collectgarbage()
collectgarbage()
collectgarbage();showmem()

local clocktime = clock() - initclock
walltime = difftime(time(), walltime)

print(format("\n\ntotal time: %.2fs (wall time: %gs)\n", clocktime, walltime))

if not usertests then
  lasttime = lasttime or clocktime    -- if no last time, ignore difference
  -- check whether current test time differs more than 5% from last time
  local diff = (clocktime - lasttime) / lasttime
  local tolerance = 0.05    -- 5%
  if (diff >= tolerance or diff <= -tolerance) then
    warn(format("#time difference from previous test: %+.1f%%",
                  diff * 100))
  end
  assert(open(fname, "w")):write(clocktime):close()
end

print("final OK !!!")



--[[
*****************************************************************************
* Copyright (C) 1994-2016 Lua.org, PUC-Rio.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*****************************************************************************
]]

