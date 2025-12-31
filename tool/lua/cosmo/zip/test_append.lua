-- Tests for cosmo.zip append mode

local zip = require("cosmo.zip")
local unix = require("cosmo.unix")

local tmpdir = unix.mkdtemp("/tmp/test_zip_append_XXXXXX")
assert(tmpdir, "failed to create temp dir")

--------------------------------------------------------------------------------
-- Test basic append functionality
--------------------------------------------------------------------------------

-- Create initial zip with write mode
local append_zip = tmpdir .. "/test_append.zip"
local writer, err = zip.open(append_zip, "w")
assert(writer, "failed to create zip for append test: " .. tostring(err))

local ok, err = writer:add("original.txt", "original content")
assert(ok, "failed to add original.txt: " .. tostring(err))

ok, err = writer:add("another.txt", "another file")
assert(ok, "failed to add another.txt: " .. tostring(err))

ok, err = writer:close()
assert(ok, "failed to close initial zip: " .. tostring(err))

-- Open in append mode and add new entries
local appender, err = zip.open(append_zip, "a")
assert(appender, "failed to open for append: " .. tostring(err))
assert(tostring(appender):match("zip.Appender"), "should be a zip.Appender")
assert(tostring(appender):match("0 pending"), "should have 0 pending entries")

ok, err = appender:add("appended.txt", "appended content")
assert(ok, "failed to add appended.txt: " .. tostring(err))
assert(tostring(appender):match("1 pending"), "should have 1 pending entry")

ok, err = appender:add("subdir/new.txt", "new file in subdir")
assert(ok, "failed to add subdir/new.txt: " .. tostring(err))

ok, err = appender:close()
assert(ok, "failed to close appender: " .. tostring(err))
assert(tostring(appender):match("closed"), "should be closed")

-- Verify all entries are present (original + appended)
local reader, err = zip.open(append_zip)
assert(reader, "failed to open appended zip: " .. tostring(err))

local entries = reader:list()
assert(#entries == 4, "should have 4 entries after append, got " .. #entries)

local entry_set = {}
for _, name in ipairs(entries) do
  entry_set[name] = true
end
assert(entry_set["original.txt"], "should have original.txt")
assert(entry_set["another.txt"], "should have another.txt")
assert(entry_set["appended.txt"], "should have appended.txt")
assert(entry_set["subdir/new.txt"], "should have subdir/new.txt")

-- Verify content is correct
local orig_content = reader:read("original.txt")
assert(orig_content == "original content", "original content should be preserved")

local appended_content = reader:read("appended.txt")
assert(appended_content == "appended content", "appended content should be correct")

reader:close()

--------------------------------------------------------------------------------
-- Test append mode with max_file_size
--------------------------------------------------------------------------------

local limit_append_zip = tmpdir .. "/test_append_limit.zip"
writer, err = zip.open(limit_append_zip, "w")
assert(writer, "failed to create zip: " .. tostring(err))
writer:add("existing.txt", "existing")
writer:close()

appender, err = zip.open(limit_append_zip, "a", {max_file_size = 50})
assert(appender, "failed to open with limit: " .. tostring(err))

ok, err = appender:add("small.txt", "small content")
assert(ok, "small content should work: " .. tostring(err))

local result, err = appender:add("big.txt", string.rep("x", 100))
assert(result == nil, "content exceeding limit should be rejected")
assert(err:match("max_file_size"), "error should mention max_file_size")

appender:close()

-- Test invalid max_file_size values for append mode
result, err = zip.open(limit_append_zip, "a", {max_file_size = 0})
assert(result == nil, "max_file_size = 0 should error")

result, err = zip.open(limit_append_zip, "a", {max_file_size = -1})
assert(result == nil, "max_file_size = -1 should error")

--------------------------------------------------------------------------------
-- Test append mode fd rejection
--------------------------------------------------------------------------------

local fd_zip = tmpdir .. "/test_fd.zip"
writer = zip.open(fd_zip, "w")
writer:add("test.txt", "test")
writer:close()

local fd = unix.open(fd_zip, unix.O_RDONLY)
assert(fd, "failed to open fd")

result, err = zip.open(fd, "a")
assert(result == nil, "fd mode for append should be rejected")
assert(err:match("file descriptor"), "error should mention file descriptor")

unix.close(fd)

--------------------------------------------------------------------------------
-- Test append to non-existent file (should create new zip)
--------------------------------------------------------------------------------

local new_append_zip = tmpdir .. "/new_append.zip"
appender, err = zip.open(new_append_zip, "a")
assert(appender, "should be able to open non-existent file for append: " .. tostring(err))

ok, err = appender:add("first.txt", "first file")
assert(ok, "should add first entry: " .. tostring(err))

ok, err = appender:close()
assert(ok, "should close appender: " .. tostring(err))

reader = zip.open(new_append_zip)
assert(reader, "should be able to read new zip")
entries = reader:list()
assert(#entries == 1, "should have 1 entry")
assert(reader:read("first.txt") == "first file", "content should match")
reader:close()

--------------------------------------------------------------------------------
-- Test append with no entries (should be no-op)
--------------------------------------------------------------------------------

local noop_zip = tmpdir .. "/noop.zip"
writer = zip.open(noop_zip, "w")
writer:add("keep.txt", "keep this")
writer:close()

appender = zip.open(noop_zip, "a")
assert(appender, "should open for append")
-- Don't add anything
ok, err = appender:close()
assert(ok, "close with no entries should succeed: " .. tostring(err))

reader = zip.open(noop_zip)
entries = reader:list()
assert(#entries == 1, "should still have 1 entry")
assert(reader:read("keep.txt") == "keep this", "original content should be preserved")
reader:close()

--------------------------------------------------------------------------------
-- Test operations on closed appender
--------------------------------------------------------------------------------

appender = zip.open(noop_zip, "a")
appender:close()

result, err = appender:add("test.txt", "content")
assert(result == nil, "add on closed appender should fail")
assert(err:match("closed"), "error should mention closed")

--------------------------------------------------------------------------------
-- Cleanup
--------------------------------------------------------------------------------

os.execute("rm -rf " .. tmpdir)

print("PASS")
