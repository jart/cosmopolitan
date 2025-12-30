-- Tests for the help module parser and functionality

local cosmo = require("cosmo")
local unix = require("cosmo.unix")
local help = require("cosmo.help")

-- Test 1: help module loaded correctly
assert(type(help) == "table", "help should be a table")
assert(type(help.show) == "function", "help.show should be a function")
assert(type(help.search) == "function", "help.search should be a function")

-- Test 2: help is callable
assert(type(getmetatable(help).__call) == "function", "help should be callable")

-- Test 3: Parser tests - trigger load
help("cosmo")
assert(type(help._docs) == "table", "help._docs should be a table after loading")

-- Test 4: Check that base definitions loaded (without cosmo. prefix)
assert(help._docs["EncodeBase64"], "EncodeBase64 should be documented")
assert(help._docs["unix.fork"], "unix.fork should be documented")
assert(help._docs["path.basename"] or help._docs["Path.basename"],
       "path.basename should be documented")

-- Test 5: Check documentation structure
local doc = help._docs["EncodeBase64"]
assert(doc.signature, "doc should have signature")
assert(doc.desc, "doc should have description")
assert(type(doc.params) == "table", "doc.params should be a table")
assert(type(doc.returns) == "table", "doc.returns should be a table")

-- Test 6: help("name") works for string lookups
local output = {}
local old_print = print
print = function(...) for i,v in ipairs({...}) do table.insert(output, tostring(v)) end end

help("EncodeBase64")
print = old_print

assert(#output > 0, "help('name') should produce output")
assert(output[1]:match("EncodeBase64"), "output should mention function name")

-- Test 7: help.search works
output = {}
print = function(...) for i,v in ipairs({...}) do table.insert(output, tostring(v)) end end

help.search("base64")
print = old_print

assert(#output > 0, "help.search should produce output")
local found_base64 = false
for _, line in ipairs(output) do
  if line:lower():match("base64") then found_base64 = true end
end
assert(found_base64, "search results should include base64 functions")

-- Test 8: Check Fetch includes proxy info
local fetch_doc = help._docs["Fetch"]
assert(fetch_doc, "Fetch should be documented")
assert(fetch_doc.desc:match("proxy") or fetch_doc.desc:match("Proxy"),
       "Fetch docs should mention proxy support")

-- Test 9: Check unix module has many functions documented
local unix_func_count = 0
for name, doc in pairs(help._docs) do
  if name:match("^unix%.") and not doc.signature:match("%(constant%)$") then
    unix_func_count = unix_func_count + 1
  end
end
assert(unix_func_count >= 20, "should have at least 20 unix functions documented, got " .. unix_func_count)

-- Test 10: Constants are documented
assert(help._docs["unix.EEXIST"], "unix.EEXIST should be documented")
assert(help._docs["unix.O_RDONLY"], "unix.O_RDONLY should be documented")
local eexist_doc = help._docs["unix.EEXIST"]
assert(eexist_doc.signature:match("%(constant%)$"), "constant signature should end with (constant)")
assert(eexist_doc.desc:match("File exists") or eexist_doc.desc:match("exists"),
       "EEXIST should describe file exists error")

-- Test 11: Unix module has many constants
local unix_const_count = 0
for name, doc in pairs(help._docs) do
  if name:match("^unix%.") and doc.signature:match("%(constant%)$") then
    unix_const_count = unix_const_count + 1
  end
end
assert(unix_const_count >= 50, "should have at least 50 unix constants documented, got " .. unix_const_count)

-- Test 12: help() is available globally
assert(_G.help == help, "help should be global")

print("all help tests passed")
