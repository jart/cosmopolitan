-- Security validation tests for cosmo.zip

local zip = require("cosmo.zip")
local unix = require("cosmo.unix")

local tmpdir = unix.mkdtemp("/tmp/test_zip_security_XXXXXX")
assert(tmpdir, "failed to create temp dir")

--------------------------------------------------------------------------------
-- Test writer security validations
--------------------------------------------------------------------------------

local sec_zip = tmpdir .. "/security_test.zip"
local writer, err = zip.open(sec_zip, "w")
assert(writer, "failed to create security test zip: " .. tostring(err))

-- Test path traversal rejection
local result, err = writer:add("../escape.txt", "malicious")
assert(result == nil, "path traversal with .. should be rejected")
assert(err:match("unsafe path"), "error should mention unsafe path")

result, err = writer:add("/absolute/path.txt", "malicious")
assert(result == nil, "absolute path should be rejected")
assert(err:match("unsafe path"), "error should mention unsafe path")

result, err = writer:add("foo/../bar.txt", "malicious")
assert(result == nil, "embedded .. should be rejected")

result, err = writer:add("foo/bar/..", "malicious")
assert(result == nil, "trailing .. should be rejected")

-- Test empty name rejection
result, err = writer:add("", "content")
assert(result == nil, "empty name should be rejected")
assert(err:match("empty"), "error should mention empty")

-- Test null byte in name rejection
result, err = writer:add("foo\0bar.txt", "content")
assert(result == nil, "null byte in name should be rejected")
assert(err:match("null"), "error should mention null")

-- Test duplicate entry rejection
local ok, err = writer:add("unique.txt", "first")
assert(ok, "first add should succeed")

result, err = writer:add("unique.txt", "second")
assert(result == nil, "duplicate entry should be rejected")
assert(err:match("duplicate"), "error should mention duplicate")

-- Test invalid mode rejection (symlink)
result, err = writer:add("symlink.txt", "content", {mode = 0120777})
assert(result == nil, "symlink mode should be rejected")
assert(err:match("regular file"), "error should mention regular file")

-- Test invalid mode rejection (directory)
result, err = writer:add("dir.txt", "content", {mode = 0040755})
assert(result == nil, "directory mode should be rejected")

-- Valid paths should still work
ok, err = writer:add("normal.txt", "content")
assert(ok, "normal path should work: " .. tostring(err))

ok, err = writer:add("subdir/file.txt", "content")
assert(ok, "subdir path should work: " .. tostring(err))

ok, err = writer:add("a/b/c/deep.txt", "content")
assert(ok, "deep path should work: " .. tostring(err))

-- Mode with just permissions (no file type) should default to regular file
ok, err = writer:add("perms.txt", "content", {mode = 0644})
assert(ok, "permission-only mode should work: " .. tostring(err))

writer:close()

--------------------------------------------------------------------------------
-- Test configurable max_file_size
--------------------------------------------------------------------------------

-- Test writer max_file_size enforcement
local limit_zip = tmpdir .. "/limit_test.zip"
writer, err = zip.open(limit_zip, "w", {max_file_size = 100})
assert(writer, "failed to create limited zip: " .. tostring(err))

ok, err = writer:add("small.txt", "hello")
assert(ok, "small content should work: " .. tostring(err))

result, err = writer:add("big.txt", string.rep("x", 200))
assert(result == nil, "content exceeding max_file_size should be rejected")
assert(err:match("max_file_size"), "error should mention max_file_size")

writer:close()

-- Test reader max_file_size enforcement
-- First create a zip with a larger file
local reader_limit_zip = tmpdir .. "/reader_limit_test.zip"
writer, err = zip.open(reader_limit_zip, "w")
assert(writer, "failed to create zip: " .. tostring(err))
ok, err = writer:add("big.txt", string.rep("y", 500))
assert(ok, "should add big file: " .. tostring(err))
writer:close()

-- Now try to read with a low limit
local reader, err = zip.open(reader_limit_zip, {max_file_size = 100})
assert(reader, "failed to open zip with limit: " .. tostring(err))

result, err = reader:read("big.txt")
assert(result == nil, "reading file exceeding max_file_size should fail")
assert(err:match("too large"), "error should mention size")

reader:close()

-- Test invalid max_file_size values
ok, err = pcall(function()
  zip.open(tmpdir .. "/bad.zip", "w", {max_file_size = 0})
end)
assert(not ok, "max_file_size = 0 should error")

ok, err = pcall(function()
  zip.open(tmpdir .. "/bad.zip", "w", {max_file_size = -1})
end)
assert(not ok, "max_file_size = -1 should error")

ok, err = pcall(function()
  zip.open(limit_zip, {max_file_size = 0})
end)
assert(not ok, "max_file_size = 0 on open should error")

--------------------------------------------------------------------------------
-- Test append mode security validations
--------------------------------------------------------------------------------

local append_zip = tmpdir .. "/append_security.zip"
writer = zip.open(append_zip, "w")
writer:add("original.txt", "original content")
writer:add("another.txt", "another file")
writer:close()

local appender, err = zip.open(append_zip, "a")
assert(appender, "failed to open for security tests: " .. tostring(err))

-- Path traversal rejection
result, err = appender:add("../escape.txt", "malicious")
assert(result == nil, "path traversal should be rejected")
assert(err:match("unsafe path"), "error should mention unsafe path")

result, err = appender:add("/absolute/path.txt", "malicious")
assert(result == nil, "absolute path should be rejected")

result, err = appender:add("foo/../bar.txt", "malicious")
assert(result == nil, "embedded .. should be rejected")

-- Empty name rejection
result, err = appender:add("", "content")
assert(result == nil, "empty name should be rejected")
assert(err:match("empty"), "error should mention empty")

-- Null byte rejection
result, err = appender:add("foo\0bar.txt", "content")
assert(result == nil, "null byte should be rejected")
assert(err:match("null"), "error should mention null")

-- Duplicate entry rejection (against existing entries)
result, err = appender:add("original.txt", "duplicate")
assert(result == nil, "duplicate of existing entry should be rejected")
assert(err:match("duplicate"), "error should mention duplicate")

-- Duplicate entry rejection (against newly added entries)
ok, err = appender:add("unique_new.txt", "first")
assert(ok, "first add should succeed: " .. tostring(err))

result, err = appender:add("unique_new.txt", "second")
assert(result == nil, "duplicate of new entry should be rejected")
assert(err:match("duplicate"), "error should mention duplicate")

appender:close()

--------------------------------------------------------------------------------
-- Cleanup
--------------------------------------------------------------------------------

os.execute("rm -rf " .. tmpdir)

print("PASS")
