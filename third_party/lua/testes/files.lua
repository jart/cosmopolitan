-- $Id: testes/files.lua $
-- See Copyright Notice in file all.lua

local debug = require "debug"

local maxint = math.maxinteger

assert(type(os.getenv"PATH") == "string")

assert(io.input(io.stdin) == io.stdin)
assert(not pcall(io.input, "non-existent-file"))
assert(io.output(io.stdout) == io.stdout)


local function testerr (msg, f, ...)
  local stat, err = pcall(f, ...)
  return (not stat and string.find(err, msg, 1, true))
end


local function checkerr (msg, f, ...)
  assert(testerr(msg, f, ...))
end


-- cannot close standard files
assert(not io.close(io.stdin) and
       not io.stdout:close() and
       not io.stderr:close())

-- cannot call close method without an argument (new in 5.3.5)
checkerr("got no value", io.stdin.close)


assert(type(io.input()) == "userdata" and io.type(io.output()) == "file")
assert(type(io.stdin) == "userdata" and io.type(io.stderr) == "file")
assert(not io.type(8))
local a = {}; setmetatable(a, {})
assert(not io.type(a))

assert(getmetatable(io.input()).__name == "FILE*")

local a,b,c = io.open('xuxu_nao_existe')
assert(not a and type(b) == "string" and type(c) == "number")

a,b,c = io.open('/a/b/c/d', 'w')
assert(not a and type(b) == "string" and type(c) == "number")

local file = os.tmpname()
local f, msg = io.open(file, "w")
if not f then
  (Message or print)("'os.tmpname' file cannot be open; skipping file tests")

else  --{  most tests here need tmpname
f:close()

print('testing i/o')

local otherfile = os.tmpname()

checkerr("invalid mode", io.open, file, "rw")
checkerr("invalid mode", io.open, file, "rb+")
checkerr("invalid mode", io.open, file, "r+bk")
checkerr("invalid mode", io.open, file, "")
checkerr("invalid mode", io.open, file, "+")
checkerr("invalid mode", io.open, file, "b")
assert(io.open(file, "r+b")):close()
assert(io.open(file, "r+")):close()
assert(io.open(file, "rb")):close()

assert(os.setlocale('C', 'all'))

io.input(io.stdin); io.output(io.stdout);

os.remove(file)
assert(not loadfile(file))
checkerr("", dofile, file)
assert(not io.open(file))
io.output(file)
assert(io.output() ~= io.stdout)

if not _port then   -- invalid seek
  local status, msg, code = io.stdin:seek("set", 1000)
  assert(not status and type(msg) == "string" and type(code) == "number")
end

assert(io.output():seek() == 0)
assert(io.write("alo alo"):seek() == string.len("alo alo"))
assert(io.output():seek("cur", -3) == string.len("alo alo")-3)
assert(io.write("joao"))
assert(io.output():seek("end") == string.len("alo joao"))

assert(io.output():seek("set") == 0)

assert(io.write('"álo"', "{a}\n", "second line\n", "third line \n"))
assert(io.write('çfourth_line'))
io.output(io.stdout)
collectgarbage()  -- file should be closed by GC
assert(io.input() == io.stdin and rawequal(io.output(), io.stdout))
print('+')

-- test GC for files
collectgarbage()
for i=1,120 do
  for i=1,5 do
    io.input(file)
    assert(io.open(file, 'r'))
    io.lines(file)
  end
  collectgarbage()
end

io.input():close()
io.close()

assert(os.rename(file, otherfile))
assert(not os.rename(file, otherfile))

io.output(io.open(otherfile, "ab"))
assert(io.write("\n\n\t\t  ", 3450, "\n"));
io.close()


do
  -- closing file by scope
  local F = nil
  do
    local f <close> = assert(io.open(file, "w"))
    F = f
  end
  assert(tostring(F) == "file (closed)")
end
assert(os.remove(file))


do
  -- test writing/reading numbers
  local f <close> = assert(io.open(file, "w"))
  f:write(maxint, '\n')
  f:write(string.format("0X%x\n", maxint))
  f:write("0xABCp-3", '\n')
  f:write(0, '\n')
  f:write(-maxint, '\n')
  f:write(string.format("0x%X\n", -maxint))
  f:write("-0xABCp-3", '\n')
  assert(f:close())
  local f <close> = assert(io.open(file, "r"))
  assert(f:read("n") == maxint)
  assert(f:read("n") == maxint)
  assert(f:read("n") == 0xABCp-3)
  assert(f:read("n") == 0)
  assert(f:read("*n") == -maxint)            -- test old format (with '*')
  assert(f:read("n") == -maxint)
  assert(f:read("*n") == -0xABCp-3)            -- test old format (with '*')
end
assert(os.remove(file))


-- testing multiple arguments to io.read
do
  local f <close> = assert(io.open(file, "w"))
  f:write[[
a line
another line
1234
3.45
one
two
three
]]
  local l1, l2, l3, l4, n1, n2, c, dummy
  assert(f:close())
  local f <close> = assert(io.open(file, "r"))
  l1, l2, n1, n2, dummy = f:read("l", "L", "n", "n")
  assert(l1 == "a line" and l2 == "another line\n" and
         n1 == 1234 and n2 == 3.45 and dummy == nil)
  assert(f:close())
  local f <close> = assert(io.open(file, "r"))
  l1, l2, n1, n2, c, l3, l4, dummy = f:read(7, "l", "n", "n", 1, "l", "l")
  assert(l1 == "a line\n" and l2 == "another line" and c == '\n' and
         n1 == 1234 and n2 == 3.45 and l3 == "one" and l4 == "two"
         and dummy == nil)
  assert(f:close())
  local f <close> = assert(io.open(file, "r"))
  -- second item failing
  l1, n1, n2, dummy = f:read("l", "n", "n", "l")
  assert(l1 == "a line" and not n1)
end
assert(os.remove(file))



-- test yielding during 'dofile'
f = assert(io.open(file, "w"))
f:write[[
local x, z = coroutine.yield(10)
local y = coroutine.yield(20)
return x + y * z
]]
assert(f:close())
f = coroutine.wrap(dofile)
assert(f(file) == 10)
assert(f(100, 101) == 20)
assert(f(200) == 100 + 200 * 101)
assert(os.remove(file))


f = assert(io.open(file, "w"))
-- test number termination
f:write[[
-12.3-	-0xffff+  .3|5.E-3X  +234e+13E 0xDEADBEEFDEADBEEFx
0x1.13Ap+3e
]]
-- very long number
f:write("1234"); for i = 1, 1000 do f:write("0") end;  f:write("\n")
-- invalid sequences (must read and discard valid prefixes)
f:write[[
.e+	0.e;	--;  0xX;
]]
assert(f:close())
f = assert(io.open(file, "r"))
assert(f:read("n") == -12.3); assert(f:read(1) == "-")
assert(f:read("n") == -0xffff); assert(f:read(2) == "+ ")
assert(f:read("n") == 0.3); assert(f:read(1) == "|")
assert(f:read("n") == 5e-3); assert(f:read(1) == "X")
assert(f:read("n") == 234e13); assert(f:read(1) == "E")
assert(f:read("n") == 0Xdeadbeefdeadbeef); assert(f:read(2) == "x\n")
assert(f:read("n") == 0x1.13aP3); assert(f:read(1) == "e")

do   -- attempt to read too long number
  assert(not f:read("n"))  -- fails
  local s = f:read("L")   -- read rest of line
  assert(string.find(s, "^00*\n$"))  -- lots of 0's left
end

assert(not f:read("n")); assert(f:read(2) == "e+")
assert(not f:read("n")); assert(f:read(1) == ";")
assert(not f:read("n")); assert(f:read(2) == "-;")
assert(not f:read("n")); assert(f:read(1) == "X")
assert(not f:read("n")); assert(f:read(1) == ";")
assert(not f:read("n")); assert(not f:read(0))   -- end of file
assert(f:close())
assert(os.remove(file))


-- test line generators
assert(not pcall(io.lines, "non-existent-file"))
assert(os.rename(otherfile, file))
io.output(otherfile)
local n = 0
local f = io.lines(file)
while f() do n = n + 1 end;
assert(n == 6)   -- number of lines in the file
checkerr("file is already closed", f)
checkerr("file is already closed", f)
-- copy from file to otherfile
n = 0
for l in io.lines(file) do io.write(l, "\n"); n = n + 1 end
io.close()
assert(n == 6)
-- copy from otherfile back to file
local f = assert(io.open(otherfile))
assert(io.type(f) == "file")
io.output(file)
assert(not io.output():read())
n = 0
for l in f:lines() do io.write(l, "\n"); n = n + 1 end
assert(tostring(f):sub(1, 5) == "file ")
assert(f:close()); io.close()
assert(n == 6)
checkerr("closed file", io.close, f)
assert(tostring(f) == "file (closed)")
assert(io.type(f) == "closed file")
io.input(file)
f = io.open(otherfile):lines()
n = 0
for l in io.lines() do assert(l == f()); n = n + 1 end
f = nil; collectgarbage()
assert(n == 6)
assert(os.remove(otherfile))

do  -- bug in 5.3.1
  io.output(otherfile)
  io.write(string.rep("a", 300), "\n")
  io.close()
  local t ={}; for i = 1, 250 do t[i] = 1 end
  t = {io.lines(otherfile, table.unpack(t))()}
  -- everything ok here
  assert(#t == 250 and t[1] == 'a' and t[#t] == 'a')
  t[#t + 1] = 1    -- one too many
  checkerr("too many arguments", io.lines, otherfile, table.unpack(t))
  collectgarbage()   -- ensure 'otherfile' is closed
  assert(os.remove(otherfile))
end

io.input(file)
do  -- test error returns
  local a,b,c = io.input():write("xuxu")
  assert(not a and type(b) == "string" and type(c) == "number")
end
checkerr("invalid format", io.read, "x")
assert(io.read(0) == "")   -- not eof
assert(io.read(5, 'l') == '"álo"')
assert(io.read(0) == "")
assert(io.read() == "second line")
local x = io.input():seek()
assert(io.read() == "third line ")
assert(io.input():seek("set", x))
assert(io.read('L') == "third line \n")
assert(io.read(1) == "ç")
assert(io.read(string.len"fourth_line") == "fourth_line")
assert(io.input():seek("cur", -string.len"fourth_line"))
assert(io.read() == "fourth_line")
assert(io.read() == "")  -- empty line
assert(io.read('n') == 3450)
assert(io.read(1) == '\n')
assert(not io.read(0))  -- end of file
assert(not io.read(1))  -- end of file
assert(not io.read(30000))  -- end of file
assert(({io.read(1)})[2] == undef)
assert(not io.read())  -- end of file
assert(({io.read()})[2] == undef)
assert(not io.read('n'))  -- end of file
assert(({io.read('n')})[2] == undef)
assert(io.read('a') == '')  -- end of file (OK for 'a')
assert(io.read('a') == '')  -- end of file (OK for 'a')
collectgarbage()
print('+')
io.close(io.input())
checkerr(" input file is closed", io.read)

assert(os.remove(file))

local t = '0123456789'
for i=1,10 do t = t..t; end
assert(string.len(t) == 10*2^10)

io.output(file)
io.write("alo"):write("\n")
io.close()
checkerr(" output file is closed", io.write)
local f = io.open(file, "a+b")
io.output(f)
collectgarbage()

assert(io.write(' ' .. t .. ' '))
assert(io.write(';', 'end of file\n'))
f:flush(); io.flush()
f:close()
print('+')

io.input(file)
assert(io.read() == "alo")
assert(io.read(1) == ' ')
assert(io.read(string.len(t)) == t)
assert(io.read(1) == ' ')
assert(io.read(0))
assert(io.read('a') == ';end of file\n')
assert(not io.read(0))
assert(io.close(io.input()))


-- test errors in read/write
do
  local function ismsg (m)
    -- error message is not a code number
    return (type(m) == "string" and not tonumber(m))
  end

  -- read
  local f = io.open(file, "w")
  local r, m, c = f:read()
  assert(not r and ismsg(m) and type(c) == "number")
  assert(f:close())
  -- write
  f = io.open(file, "r")
  r, m, c = f:write("whatever")
  assert(not r and ismsg(m) and type(c) == "number")
  assert(f:close())
  -- lines
  f = io.open(file, "w")
  r, m = pcall(f:lines())
  assert(r == false and ismsg(m))
  assert(f:close())
end

assert(os.remove(file))

-- test for L format
io.output(file); io.write"\n\nline\nother":close()
io.input(file)
assert(io.read"L" == "\n")
assert(io.read"L" == "\n")
assert(io.read"L" == "line\n")
assert(io.read"L" == "other")
assert(not io.read"L")
io.input():close()

local f = assert(io.open(file))
local s = ""
for l in f:lines("L") do s = s .. l end
assert(s == "\n\nline\nother")
f:close()

io.input(file)
s = ""
for l in io.lines(nil, "L") do s = s .. l end
assert(s == "\n\nline\nother")
io.input():close()

s = ""
for l in io.lines(file, "L") do s = s .. l end
assert(s == "\n\nline\nother")

s = ""
for l in io.lines(file, "l") do s = s .. l end
assert(s == "lineother")

io.output(file); io.write"a = 10 + 34\na = 2*a\na = -a\n":close()
local t = {}
assert(load(io.lines(file, "L"), nil, nil, t))()
assert(t.a == -((10 + 34) * 2))


do   -- testing closing file in line iteration

  -- get the to-be-closed variable from a loop
  local function gettoclose (lv)
    lv = lv + 1
    local stvar = 0   -- to-be-closed is 4th state variable in the loop
    for i = 1, 1000 do
      local n, v = debug.getlocal(lv, i)
      if n == "(for state)" then
        stvar = stvar + 1
        if stvar == 4 then return v end
      end
    end
  end

  local f
  for l in io.lines(file) do
    f = gettoclose(1)
    assert(io.type(f) == "file")
    break
  end
  assert(io.type(f) == "closed file")

  f = nil
  local function foo (name)
    for l in io.lines(name) do
      f = gettoclose(1)
      assert(io.type(f) == "file")
      error(f)   -- exit loop with an error
    end
  end
  local st, msg = pcall(foo, file)
  assert(st == false and io.type(msg) == "closed file")

end


-- test for multipe arguments in 'lines'
io.output(file); io.write"0123456789\n":close()
for a,b in io.lines(file, 1, 1) do
  if a == "\n" then assert(not b)
  else assert(tonumber(a) == tonumber(b) - 1)
  end
end

for a,b,c in io.lines(file, 1, 2, "a") do
  assert(a == "0" and b == "12" and c == "3456789\n")
end

for a,b,c in io.lines(file, "a", 0, 1) do
  if a == "" then break end
  assert(a == "0123456789\n" and not b and not c)
end
collectgarbage()   -- to close file in previous iteration

io.output(file); io.write"00\n10\n20\n30\n40\n":close()
for a, b in io.lines(file, "n", "n") do
  if a == 40 then assert(not b)
  else assert(a == b - 10)
  end
end


-- test load x lines
io.output(file);
io.write[[
local y
= X
X =
X *
2 +
X;
X =
X
-                                   y;
]]:close()
_G.X = 1
assert(not load((io.lines(file))))
collectgarbage()   -- to close file in previous iteration
load((io.lines(file, "L")))()
assert(_G.X == 2)
load((io.lines(file, 1)))()
assert(_G.X == 4)
load((io.lines(file, 3)))()
assert(_G.X == 8)

print('+')

local x1 = "string\n\n\\com \"\"''coisas [[estranhas]] ]]'"
io.output(file)
assert(io.write(string.format("x2 = %q\n-- comment without ending EOS", x1)))
io.close()
assert(loadfile(file))()
assert(x1 == x2)
print('+')
assert(os.remove(file))
assert(not os.remove(file))
assert(not os.remove(otherfile))

-- testing loadfile
local function testloadfile (s, expres)
  io.output(file)
  if s then io.write(s) end
  io.close()
  local res = assert(loadfile(file))()
  assert(os.remove(file))
  assert(res == expres)
end

-- loading empty file
testloadfile(nil, nil)

-- loading file with initial comment without end of line
testloadfile("# a non-ending comment", nil)


-- checking Unicode BOM in files
testloadfile("\xEF\xBB\xBF# some comment\nreturn 234", 234)
testloadfile("\xEF\xBB\xBFreturn 239", 239)
testloadfile("\xEF\xBB\xBF", nil)   -- empty file with a BOM


-- checking line numbers in files with initial comments
testloadfile("# a comment\nreturn require'debug'.getinfo(1).currentline", 2)


-- loading binary file
io.output(io.open(file, "wb"))
assert(io.write(string.dump(function () return 10, '\0alo\255', 'hi' end)))
io.close()
a, b, c = assert(loadfile(file))()
assert(a == 10 and b == "\0alo\255" and c == "hi")
assert(os.remove(file))

-- bug in 5.2.1
do
  io.output(io.open(file, "wb"))
  -- save function with no upvalues
  assert(io.write(string.dump(function () return 1 end)))
  io.close()
  f = assert(loadfile(file, "b", {}))
  assert(type(f) == "function" and f() == 1)
  assert(os.remove(file))
end

-- loading binary file with initial comment
io.output(io.open(file, "wb"))
assert(io.write("#this is a comment for a binary file\0\n",
                string.dump(function () return 20, '\0\0\0' end)))
io.close()
a, b, c = assert(loadfile(file))()
assert(a == 20 and b == "\0\0\0" and c == nil)
assert(os.remove(file))


-- 'loadfile' with 'env'
do
  local f = io.open(file, 'w')
  f:write[[
    if (...) then a = 15; return b, c, d
    else return _ENV
    end
  ]]
  f:close()
  local t = {b = 12, c = "xuxu", d = print}
  local f = assert(loadfile(file, 't', t))
  local b, c, d = f(1)
  assert(t.a == 15 and b == 12 and c == t.c and d == print)
  assert(f() == t)
  f = assert(loadfile(file, 't', nil))
  assert(f() == nil)
  f = assert(loadfile(file))
  assert(f() == _G)
  assert(os.remove(file))
end


-- 'loadfile' x modes
do
  io.open(file, 'w'):write("return 10"):close()
  local s, m = loadfile(file, 'b')
  assert(not s and string.find(m, "a text chunk"))
  io.open(file, 'w'):write("\27 return 10"):close()
  local s, m = loadfile(file, 't')
  assert(not s and string.find(m, "a binary chunk"))
  assert(os.remove(file))
end


io.output(file)
assert(io.write("qualquer coisa\n"))
assert(io.write("mais qualquer coisa"))
io.close()
assert(io.output(assert(io.open(otherfile, 'wb')))
       :write("outra coisa\0\1\3\0\0\0\0\255\0")
       :close())

local filehandle = assert(io.open(file, 'r+'))
local otherfilehandle = assert(io.open(otherfile, 'rb'))
assert(filehandle ~= otherfilehandle)
assert(type(filehandle) == "userdata")
assert(filehandle:read('l') == "qualquer coisa")
io.input(otherfilehandle)
assert(io.read(string.len"outra coisa") == "outra coisa")
assert(filehandle:read('l') == "mais qualquer coisa")
filehandle:close();
assert(type(filehandle) == "userdata")
io.input(otherfilehandle)
assert(io.read(4) == "\0\1\3\0")
assert(io.read(3) == "\0\0\0")
assert(io.read(0) == "")        -- 255 is not eof
assert(io.read(1) == "\255")
assert(io.read('a') == "\0")
assert(not io.read(0))
assert(otherfilehandle == io.input())
otherfilehandle:close()
assert(os.remove(file))
assert(os.remove(otherfile))
collectgarbage()

io.output(file)
  :write[[
 123.4	-56e-2  not a number
second line
third line

and the rest of the file
]]
  :close()
io.input(file)
local _,a,b,c,d,e,h,__ = io.read(1, 'n', 'n', 'l', 'l', 'l', 'a', 10)
assert(io.close(io.input()))
assert(_ == ' ' and not __)
assert(type(a) == 'number' and a==123.4 and b==-56e-2)
assert(d=='second line' and e=='third line')
assert(h==[[

and the rest of the file
]])
assert(os.remove(file))
collectgarbage()

-- testing buffers
do
  local f = assert(io.open(file, "w"))
  local fr = assert(io.open(file, "r"))
  assert(f:setvbuf("full", 2000))
  f:write("x")
  assert(fr:read("all") == "")  -- full buffer; output not written yet
  f:close()
  fr:seek("set")
  assert(fr:read("all") == "x")   -- `close' flushes it
  f = assert(io.open(file), "w")
  assert(f:setvbuf("no"))
  f:write("x")
  fr:seek("set")
  assert(fr:read("all") == "x")  -- no buffer; output is ready
  f:close()
  f = assert(io.open(file, "a"))
  assert(f:setvbuf("line"))
  f:write("x")
  fr:seek("set", 1)
  assert(fr:read("all") == "")   -- line buffer; no output without `\n'
  f:write("a\n"):seek("set", 1)
  assert(fr:read("all") == "xa\n")  -- now we have a whole line
  f:close(); fr:close()
  assert(os.remove(file))
end


if not _soft then
  print("testing large files (> BUFSIZ)")
  io.output(file)
  for i=1,5001 do io.write('0123456789123') end
  io.write('\n12346'):close()
  io.input(file)
  local x = io.read('a')
  io.input():seek('set', 0)
  local y = io.read(30001)..io.read(1005)..io.read(0)..
            io.read(1)..io.read(100003)
  assert(x == y and string.len(x) == 5001*13 + 6)
  io.input():seek('set', 0)
  y = io.read()  -- huge line
  assert(x == y..'\n'..io.read())
  assert(not io.read())
  io.close(io.input())
  assert(os.remove(file))
  x = nil; y = nil
end

if not _port then
  local progname
  do  -- get name of running executable
    local arg = arg or ARG
    local i = 0
    while arg[i] do i = i - 1 end
    progname = '"' .. arg[i + 1] .. '"'
  end
  print("testing popen/pclose and execute")
  -- invalid mode for popen
  checkerr("invalid mode", io.popen, "cat", "")
  checkerr("invalid mode", io.popen, "cat", "r+")
  checkerr("invalid mode", io.popen, "cat", "rw")
  do  -- basic tests for popen
    local file = os.tmpname()
    local f = assert(io.popen("cat - > " .. file, "w"))
    f:write("a line")
    assert(f:close())
    local f = assert(io.popen("cat - < " .. file, "r"))
    assert(f:read("a") == "a line")
    assert(f:close())
    assert(os.remove(file))
  end

  local tests = {
    -- command,   what,  code
    {"ls > /dev/null", "ok"},
    {"not-to-be-found-command", "exit"},
    {"exit 3", "exit", 3},
    {"exit 129", "exit", 129},
    {"kill -s HUP $$", "signal", 1},
    {"kill -s KILL $$", "signal", 9},
    {"sh -c 'kill -s HUP $$'", "exit"},
    {progname .. ' -e " "', "ok"},
    {progname .. ' -e "os.exit(0, true)"', "ok"},
    {progname .. ' -e "os.exit(20, true)"', "exit", 20},
  }
  print("\n(some error messages are expected now)")
  for _, v in ipairs(tests) do
    local x, y, z = io.popen(v[1]):close()
    local x1, y1, z1 = os.execute(v[1])
    assert(x == x1 and y == y1 and z == z1)
    if v[2] == "ok" then
      assert(x and y == 'exit' and z == 0)
    else
      assert(not x and y == v[2])   -- correct status and 'what'
      -- correct code if known (but always different from 0)
      assert((v[3] == nil and z > 0) or v[3] == z)
    end
  end
end


-- testing tmpfile
f = io.tmpfile()
assert(io.type(f) == "file")
f:write("alo")
f:seek("set")
assert(f:read"a" == "alo")

end --}

print'+'

print("testing date/time")

assert(os.date("") == "")
assert(os.date("!") == "")
assert(os.date("\0\0") == "\0\0")
assert(os.date("!\0\0") == "\0\0")
local x = string.rep("a", 10000)
assert(os.date(x) == x)
local t = os.time()
D = os.date("*t", t)
assert(os.date(string.rep("%d", 1000), t) ==
       string.rep(os.date("%d", t), 1000))
assert(os.date(string.rep("%", 200)) == string.rep("%", 100))

local function checkDateTable (t)
  _G.D = os.date("*t", t)
  assert(os.time(D) == t)
  load(os.date([[assert(D.year==%Y and D.month==%m and D.day==%d and
    D.hour==%H and D.min==%M and D.sec==%S and
    D.wday==%w+1 and D.yday==%j)]], t))()
  _G.D = nil
end

checkDateTable(os.time())
if not _port then
  -- assume that time_t can represent these values
  checkDateTable(0)
  checkDateTable(1)
  checkDateTable(1000)
  checkDateTable(0x7fffffff)
  checkDateTable(0x80000000)
end

checkerr("invalid conversion specifier", os.date, "%")
checkerr("invalid conversion specifier", os.date, "%9")
checkerr("invalid conversion specifier", os.date, "%")
checkerr("invalid conversion specifier", os.date, "%O")
checkerr("invalid conversion specifier", os.date, "%E")
checkerr("invalid conversion specifier", os.date, "%Ea")

checkerr("not an integer", os.time, {year=1000, month=1, day=1, hour='x'})
checkerr("not an integer", os.time, {year=1000, month=1, day=1, hour=1.5})

checkerr("missing", os.time, {hour = 12})   -- missing date


if string.packsize("i") == 4 then   -- 4-byte ints
  checkerr("field 'year' is out-of-bound", os.time,
              {year = -(1 << 31) + 1899, month = 1, day = 1})
end

if not _port then
  -- test Posix-specific modifiers
  assert(type(os.date("%Ex")) == 'string')
  assert(type(os.date("%Oy")) == 'string')

  -- test large dates (assume at least 4-byte ints and time_t)
  local t0 = os.time{year = 1970, month = 1, day = 0}
  local t1 = os.time{year = 1970, month = 1, day = 0, sec = (1 << 31) - 1}
  assert(t1 - t0 == (1 << 31) - 1)
  t0 = os.time{year = 1970, month = 1, day = 1}
  t1 = os.time{year = 1970, month = 1, day = 1, sec = -(1 << 31)}
  assert(t1 - t0 == -(1 << 31))

  -- test out-of-range dates (at least for Unix)
  if maxint >= 2^62 then  -- cannot do these tests in Small Lua
    -- no arith overflows
    checkerr("out-of-bound", os.time, {year = -maxint, month = 1, day = 1})
    if string.packsize("i") == 4 then   -- 4-byte ints
      if testerr("out-of-bound", os.date, "%Y", 2^40) then
        -- time_t has 4 bytes and therefore cannot represent year 4000
        print("  4-byte time_t")
        checkerr("cannot be represented", os.time, {year=4000, month=1, day=1})
      else
        -- time_t has 8 bytes; an int year cannot represent a huge time
        print("  8-byte time_t")
        checkerr("cannot be represented", os.date, "%Y", 2^60)

        -- this is the maximum year
        assert(tonumber(os.time
          {year=(1 << 31) + 1899, month=12, day=31, hour=23, min=59, sec=59}))

        -- this is too much
        checkerr("represented", os.time,
          {year=(1 << 31) + 1899, month=12, day=31, hour=23, min=59, sec=60})
      end

      -- internal 'int' fields cannot hold these values
      checkerr("field 'day' is out-of-bound", os.time,
                  {year = 0, month = 1, day = 2^32})

      checkerr("field 'month' is out-of-bound", os.time,
                  {year = 0, month = -((1 << 31) + 1), day = 1})

      checkerr("field 'year' is out-of-bound", os.time,
                  {year = (1 << 31) + 1900, month = 1, day = 1})

    else    -- 8-byte ints
      -- assume time_t has 8 bytes too
      print("  8-byte time_t")
      assert(tonumber(os.date("%Y", 2^60)))

      -- but still cannot represent a huge year
      checkerr("cannot be represented", os.time, {year=2^60, month=1, day=1})
    end
  end
end

do
  local D = os.date("*t")
  local t = os.time(D)
  if D.isdst == nil then
    print("no daylight saving information")
  else
    assert(type(D.isdst) == 'boolean')
  end
  D.isdst = nil
  local t1 = os.time(D)
  assert(t == t1)   -- if isdst is absent uses correct default
end

local D = os.date("*t")
t = os.time(D)
D.year = D.year-1;
local t1 = os.time(D)
-- allow for leap years
assert(math.abs(os.difftime(t,t1)/(24*3600) - 365) < 2)

-- should not take more than 1 second to execute these two lines
t = os.time()
t1 = os.time(os.date("*t"))
local diff = os.difftime(t1,t)
assert(0 <= diff and diff <= 1)
diff = os.difftime(t,t1)
assert(-1 <= diff and diff <= 0)

local t1 = os.time{year=2000, month=10, day=1, hour=23, min=12}
local t2 = os.time{year=2000, month=10, day=1, hour=23, min=10, sec=19}
assert(os.difftime(t1,t2) == 60*2-19)

-- since 5.3.3, 'os.time' normalizes table fields
t1 = {year = 2005, month = 1, day = 1, hour = 1, min = 0, sec = -3602}
os.time(t1)
assert(t1.day == 31 and t1.month == 12 and t1.year == 2004 and
       t1.hour == 23 and t1.min == 59 and t1.sec == 58 and
       t1.yday == 366)

io.output(io.stdout)
local t = os.date('%d %m %Y %H %M %S')
local d, m, a, h, min, s = string.match(t,
                             "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
d = tonumber(d)
m = tonumber(m)
a = tonumber(a)
h = tonumber(h)
min = tonumber(min)
s = tonumber(s)
io.write(string.format('test done on %2.2d/%2.2d/%d', d, m, a))
io.write(string.format(', at %2.2d:%2.2d:%2.2d\n', h, min, s))
io.write(string.format('%s\n', _VERSION))


