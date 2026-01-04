local getopt = require("cosmo.getopt")

-- Test module exists
assert(getopt, "getopt module should exist")
assert(type(getopt.new) == "function", "getopt.new should be a function")

-- Test short options only
local parser = getopt.new({"-h", "-v", "-o", "out.txt", "file.txt"}, "hvo:")
local opts = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  opts[opt] = arg or true
end
assert(opts.h == true, "opts.h should be true")
assert(opts.v == true, "opts.v should be true")
assert(opts.o == "out.txt", "opts.o should be 'out.txt'")
local args = parser:remaining()
assert(args[1] == "file.txt", "first remaining arg should be 'file.txt'")
assert(#args == 1, "should have exactly 1 remaining arg")
local unknown = parser:unknown()
assert(#unknown == 0, "should have no unknown options")

-- Test long options
parser = getopt.new({"--help", "--output=foo.txt", "input.txt"}, "ho:", {
  {"help", "none", "h"},
  {"output", "required", "o"},
})
opts = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  -- Map long options to their short equivalents for easier handling
  if opt == "help" then opt = "h" end
  if opt == "output" then opt = "o" end
  opts[opt] = arg or true
end
assert(opts.h == true, "opts.h should be true")
assert(opts.o == "foo.txt", "opts.o should be 'foo.txt'")
args = parser:remaining()
assert(args[1] == "input.txt", "remaining arg should be 'input.txt'")
unknown = parser:unknown()
assert(#unknown == 0, "should have no unknown options")

-- Test long option with separate argument
parser = getopt.new({"--output", "bar.txt"}, "o:", {
  {"output", "required", "o"},
})
opts = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  if opt == "output" then opt = "o" end
  opts[opt] = arg or true
end
assert(opts.o == "bar.txt", "opts.o should be 'bar.txt'")
unknown = parser:unknown()
assert(#unknown == 0, "should have no unknown options")

-- Test combined short options
parser = getopt.new({"-hv", "file.txt"}, "hv")
opts = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  opts[opt] = arg or true
end
assert(opts.h == true, "opts.h should be true")
assert(opts.v == true, "opts.v should be true")
args = parser:remaining()
assert(args[1] == "file.txt", "remaining arg should be 'file.txt'")
unknown = parser:unknown()
assert(#unknown == 0, "should have no unknown options")

-- Test -- terminator
parser = getopt.new({"-h", "--", "-v", "file.txt"}, "hv")
opts = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  opts[opt] = arg or true
end
assert(opts.h == true, "opts.h should be true")
assert(opts.v == nil, "-v after -- should not be parsed as option")
args = parser:remaining()
assert(args[1] == "-v", "first remaining should be '-v'")
assert(args[2] == "file.txt", "second remaining should be 'file.txt'")
unknown = parser:unknown()
assert(#unknown == 0, "should have no unknown options")

-- Test empty args
parser = getopt.new({}, "hv")
opts = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  opts[opt] = arg or true
end
assert(next(opts) == nil, "opts should be empty")
args = parser:remaining()
assert(#args == 0, "args should be empty")
unknown = parser:unknown()
assert(#unknown == 0, "should have no unknown options")

-- Test no longopts argument
parser = getopt.new({"-h"}, "h")
opts = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  opts[opt] = arg or true
end
assert(opts.h == true, "opts.h should be true without longopts arg")
unknown = parser:unknown()
assert(#unknown == 0, "should have no unknown options")

-- Test unknown short option
parser = getopt.new({"-h", "-x", "-v"}, "hv")
opts = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  if opt == "?" then
    -- Skip unknown options
  else
    opts[opt] = arg or true
  end
end
assert(opts.h == true, "opts.h should be true")
assert(opts.v == true, "opts.v should be true")
assert(opts.x == nil, "opts.x should be nil (unknown)")
unknown = parser:unknown()
assert(#unknown == 1, "should have 1 unknown option")
assert(unknown[1] == "x", "unknown[1] should be 'x'")

-- Test unknown long option
parser = getopt.new({"--help", "--foo", "--verbose"}, "hv", {
  {"help", "none", "h"},
  {"verbose", "none", "v"},
})
opts = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  if opt == "?" then
    -- Skip unknown options
  else
    if opt == "help" then opt = "h" end
    if opt == "verbose" then opt = "v" end
    opts[opt] = arg or true
  end
end
assert(opts.h == true, "opts.h should be true")
assert(opts.v == true, "opts.v should be true")
unknown = parser:unknown()
assert(#unknown == 1, "should have 1 unknown option")

-- Test multiple unknown options
parser = getopt.new({"-x", "-y", "-z"}, "h")
while true do
  local opt, arg = parser:next()
  if not opt then break end
end
unknown = parser:unknown()
assert(#unknown == 3, "should have 3 unknown options")

-- Test repeated short options
parser = getopt.new({"-e", "foo", "-e", "bar", "-e", "spam"}, "e:")
local e_values = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  if opt == "e" then
    table.insert(e_values, arg)
  end
end
assert(#e_values == 3, "should have 3 values for -e")
assert(e_values[1] == "foo", "first -e value should be 'foo'")
assert(e_values[2] == "bar", "second -e value should be 'bar'")
assert(e_values[3] == "spam", "third -e value should be 'spam'")

-- Test repeated long options
parser = getopt.new({"--include", "a.h", "--include", "b.h"}, "I:", {
  {"include", "required", "I"},
})
local include_values = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  if opt == "include" or opt == "I" then
    table.insert(include_values, arg)
  end
end
assert(#include_values == 2, "should have 2 values for --include")
assert(include_values[1] == "a.h", "first --include should be 'a.h'")
assert(include_values[2] == "b.h", "second --include should be 'b.h'")

-- Test mix of single and repeated options
parser = getopt.new({"-v", "-e", "foo", "-o", "out", "-e", "bar"}, "ve:o:")
opts = {}
e_values = {}
while true do
  local opt, arg = parser:next()
  if not opt then break end
  if opt == "e" then
    table.insert(e_values, arg)
  else
    opts[opt] = arg or true
  end
end
assert(opts.v == true, "single option should be boolean")
assert(#e_values == 2, "should have 2 values for repeated -e")
assert(e_values[1] == "foo" and e_values[2] == "bar", "repeated values should be correct")
assert(opts.o == "out", "single option with arg should be string")

print("PASS")
