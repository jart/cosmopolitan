#!/usr/bin/env lua
-- Test script for cosmo.repl module

print("Testing cosmo.repl module...")

-- Try to require the module
local ok, repl = pcall(require, "cosmo.repl")

if not ok then
  print("ERROR: Failed to load cosmo.repl module:")
  print(repl)
  os.exit(1)
end

print("SUCCESS: cosmo.repl module loaded successfully")

-- Check that the start function exists
if type(repl.start) ~= "function" then
  print("ERROR: repl.start is not a function")
  os.exit(1)
end

print("SUCCESS: repl.start function exists")

-- Note: We can't actually test the REPL interactively here,
-- but we've verified the module loads and the function exists

print("\nAll tests passed!")
print("\nTo test the REPL interactively, run:")
print([[  lua -e "require('cosmo.repl').start()"]])
