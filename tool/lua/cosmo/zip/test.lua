-- Basic tests for cosmo.zip reader and writer

local zip = require("cosmo.zip")
local unix = require("cosmo.unix")

local tmpdir = unix.mkdtemp("/tmp/test_zip_XXXXXX")
assert(tmpdir, "failed to create temp dir")

--------------------------------------------------------------------------------
-- Test module exists
--------------------------------------------------------------------------------

assert(zip, "zip module should exist")
assert(type(zip.open) == "function", "zip.open should be a function")

--------------------------------------------------------------------------------
-- Test zip.open (Reader) with system-created zip
--------------------------------------------------------------------------------

local file1_content = "Hello, World!"
local file2_content = "This is a longer text file with some content.\nIt has multiple lines.\nAnd some more text."

-- Write test files
local fd = unix.open(tmpdir .. "/file1.txt", unix.O_CREAT | unix.O_WRONLY, 0644)
assert(fd, "failed to create file1.txt")
unix.write(fd, file1_content)
unix.close(fd)

fd = unix.open(tmpdir .. "/file2.txt", unix.O_CREAT | unix.O_WRONLY, 0644)
assert(fd, "failed to create file2.txt")
unix.write(fd, file2_content)
unix.close(fd)

-- Create zip file using system zip command
local zippath = tmpdir .. "/test.zip"
local ok = os.execute("cd " .. tmpdir .. " && zip -q test.zip file1.txt file2.txt")
assert(ok == 0 or ok == true, "failed to create test zip file")

-- Test opening the zip file
local reader, err = zip.open(zippath)
assert(reader, "failed to open zip: " .. tostring(err))
assert(tostring(reader):match("zip.Reader"), "tostring should identify as zip.Reader")

-- Test listing entries
local entries = reader:list()
assert(entries, "list should return a table")
assert(#entries == 2, "should have 2 entries, got " .. #entries)

local has_file1, has_file2 = false, false
for _, name in ipairs(entries) do
  if name == "file1.txt" then has_file1 = true end
  if name == "file2.txt" then has_file2 = true end
end
assert(has_file1, "should have file1.txt")
assert(has_file2, "should have file2.txt")

-- Test stat
local stat = reader:stat("file1.txt")
assert(stat, "stat should return a table for existing file")
assert(stat.size == #file1_content, "size should match content length")
assert(stat.crc32, "should have crc32")
assert(stat.method ~= nil, "should have method")
assert(stat.mtime, "should have mtime")

-- Test stat for non-existent file
local stat2 = reader:stat("nonexistent.txt")
assert(stat2 == nil, "stat should return nil for non-existent file")

-- Test reading uncompressed content
local content1 = reader:read("file1.txt")
assert(content1 == file1_content, "read content should match original: got '" .. tostring(content1) .. "'")

-- Test reading compressed content (file2 might be compressed if large enough)
local content2 = reader:read("file2.txt")
assert(content2 == file2_content, "read content should match original for file2")

-- Test reading non-existent file
local content3, err3 = reader:read("nonexistent.txt")
assert(content3 == nil, "reading non-existent file should return nil")
assert(err3, "should have error message")

-- Test close
reader:close()
assert(tostring(reader):match("closed"), "tostring should indicate closed after close()")

-- Test operations on closed reader
local entries2, err2 = reader:list()
assert(entries2 == nil, "list on closed reader should return nil")
assert(err2, "should have error message for closed reader")

-- Test opening non-existent file
local reader2, err2 = zip.open("/nonexistent/path/to/file.zip")
assert(reader2 == nil, "opening non-existent file should return nil")
assert(err2, "should have error message")

-- Test opening non-zip file
local reader3, err3 = zip.open(tmpdir .. "/file1.txt")
assert(reader3 == nil, "opening non-zip file should return nil")
assert(err3, "should have error message for non-zip file")

--------------------------------------------------------------------------------
-- Test zip.open write mode (Writer)
--------------------------------------------------------------------------------

local zippath_writer = tmpdir .. "/test_writer.zip"
local writer_file1_content = "Hello, World!"
local writer_file2_content = "This is a longer text file with some content.\nIt has multiple lines.\nAnd some more text to ensure compression is worthwhile."

-- Test creating a new zip file
local writer, err = zip.open(zippath_writer, "w")
assert(writer, "failed to create zip: " .. tostring(err))
assert(tostring(writer):match("zip.Writer"), "tostring should identify as zip.Writer")
assert(tostring(writer):match("0 entries"), "new writer should have 0 entries")

-- Test adding files
ok, err = writer:add("file1.txt", writer_file1_content)
assert(ok, "failed to add file1.txt: " .. tostring(err))
assert(tostring(writer):match("1 entries"), "writer should have 1 entry after add")

ok, err = writer:add("subdir/file2.txt", writer_file2_content)
assert(ok, "failed to add subdir/file2.txt: " .. tostring(err))
assert(tostring(writer):match("2 entries"), "writer should have 2 entries after add")

-- Test adding with options
ok, err = writer:add("stored.txt", "stored content", {method = "store"})
assert(ok, "failed to add stored.txt: " .. tostring(err))

ok, err = writer:add("with_time.txt", "timed content", {mtime = 1700000000})
assert(ok, "failed to add with_time.txt: " .. tostring(err))

-- Test closing the writer
ok, err = writer:close()
assert(ok, "failed to close zip: " .. tostring(err))
assert(tostring(writer):match("closed"), "tostring should indicate closed")

-- Test operations on closed writer
local result, err = writer:add("test.txt", "content")
assert(result == nil, "add on closed writer should return nil")
assert(err, "should have error message for closed writer")

--------------------------------------------------------------------------------
-- Test reading back the zip we just wrote
--------------------------------------------------------------------------------

reader, err = zip.open(zippath_writer)
assert(reader, "failed to open zip we just wrote: " .. tostring(err))

-- Test listing entries
entries = reader:list()
assert(entries, "list should return a table")
assert(#entries == 4, "should have 4 entries, got " .. #entries)

has_file1, has_file2, has_stored, has_timed = false, false, false, false
for _, name in ipairs(entries) do
  if name == "file1.txt" then has_file1 = true end
  if name == "subdir/file2.txt" then has_file2 = true end
  if name == "stored.txt" then has_stored = true end
  if name == "with_time.txt" then has_timed = true end
end
assert(has_file1, "should have file1.txt")
assert(has_file2, "should have subdir/file2.txt")
assert(has_stored, "should have stored.txt")
assert(has_timed, "should have with_time.txt")

-- Test reading content
content1 = reader:read("file1.txt")
assert(content1 == writer_file1_content, "file1.txt content should match: got '" .. tostring(content1) .. "'")

content2 = reader:read("subdir/file2.txt")
assert(content2 == writer_file2_content, "subdir/file2.txt content should match")

local content_stored = reader:read("stored.txt")
assert(content_stored == "stored content", "stored.txt content should match")

-- Test stat
stat = reader:stat("file1.txt")
assert(stat, "stat should return a table")
assert(stat.size == #writer_file1_content, "size should match content length")
assert(stat.crc32, "should have crc32")

stat2 = reader:stat("stored.txt")
assert(stat2, "stat for stored.txt should work")
assert(stat2.method == 0, "stored file should have method 0 (store)")

reader:close()

--------------------------------------------------------------------------------
-- Test compression levels
--------------------------------------------------------------------------------

local zippath2 = tmpdir .. "/test_levels.zip"
writer, err = zip.open(zippath2, "w", {level = 9})
assert(writer, "failed to create zip with level 9: " .. tostring(err))

local big_content = string.rep("This is repetitive text. ", 1000)
ok, err = writer:add("big.txt", big_content)
assert(ok, "failed to add big.txt: " .. tostring(err))

ok, err = writer:close()
assert(ok, "failed to close: " .. tostring(err))

reader, err = zip.open(zippath2)
assert(reader, "failed to open level 9 zip: " .. tostring(err))

local stat_big = reader:stat("big.txt")
assert(stat_big, "stat should work for big.txt")
assert(stat_big.size == #big_content, "uncompressed size should match")
assert(stat_big.compressed_size < stat_big.size, "compressed size should be smaller for repetitive content")

local read_big = reader:read("big.txt")
assert(read_big == big_content, "content should match after compression/decompression")

reader:close()

--------------------------------------------------------------------------------
-- Test zip.open write mode with level 0 (store only)
--------------------------------------------------------------------------------

local zippath3 = tmpdir .. "/test_store.zip"
writer, err = zip.open(zippath3, "w", {level = 0})
assert(writer, "failed to create store-only zip: " .. tostring(err))

ok, err = writer:add("data.txt", big_content)
assert(ok, "failed to add data.txt: " .. tostring(err))

ok, err = writer:close()
assert(ok, "failed to close: " .. tostring(err))

reader, err = zip.open(zippath3)
assert(reader, "failed to open store-only zip: " .. tostring(err))

local stat_store = reader:stat("data.txt")
assert(stat_store.method == 0, "should use store method with level 0")
assert(stat_store.compressed_size == stat_store.size, "stored file should have same compressed and uncompressed size")

reader:close()

--------------------------------------------------------------------------------
-- Test edge cases
--------------------------------------------------------------------------------

-- Empty file
local zippath4 = tmpdir .. "/test_empty.zip"
writer, err = zip.open(zippath4, "w")
assert(writer, "failed to create zip: " .. tostring(err))

ok, err = writer:add("empty.txt", "")
assert(ok, "failed to add empty file: " .. tostring(err))

ok, err = writer:close()
assert(ok, "failed to close: " .. tostring(err))

reader, err = zip.open(zippath4)
assert(reader, "failed to open zip: " .. tostring(err))

local stat_empty = reader:stat("empty.txt")
assert(stat_empty.size == 0, "empty file should have size 0")

local content_empty = reader:read("empty.txt")
assert(content_empty == "", "empty file should read as empty string")

reader:close()

-- Binary content
local zippath5 = tmpdir .. "/test_binary.zip"
writer, err = zip.open(zippath5, "w")
assert(writer, "failed to create zip: " .. tostring(err))

local binary_content = ""
for i = 0, 255 do
  binary_content = binary_content .. string.char(i)
end

ok, err = writer:add("binary.bin", binary_content)
assert(ok, "failed to add binary file: " .. tostring(err))

ok, err = writer:close()
assert(ok, "failed to close: " .. tostring(err))

reader, err = zip.open(zippath5)
assert(reader, "failed to open zip: " .. tostring(err))

local read_binary = reader:read("binary.bin")
assert(read_binary == binary_content, "binary content should round-trip correctly")

reader:close()

--------------------------------------------------------------------------------
-- Test error cases
--------------------------------------------------------------------------------

-- Creating in non-existent directory
local bad_writer, bad_err = zip.open("/nonexistent/path/to/file.zip", "w")
assert(bad_writer == nil, "creating in non-existent dir should fail")
assert(bad_err, "should have error message")

-- Invalid compression level
ok, err = pcall(function()
  zip.open(tmpdir .. "/bad.zip", "w", {level = 10})
end)
assert(not ok, "level 10 should error")

--------------------------------------------------------------------------------
-- Cleanup
--------------------------------------------------------------------------------

os.execute("rm -rf " .. tmpdir)

print("PASS")
