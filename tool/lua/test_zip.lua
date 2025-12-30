local zip = require("cosmo.zip")
local unix = require("cosmo.unix")

-- Test module exists
assert(zip, "zip module should exist")
assert(type(zip.open) == "function", "zip.open should be a function")

-- Create a temporary test zip file
local tmpdir = unix.mkdtemp("/tmp/test_zip_XXXXXX")
assert(tmpdir, "failed to create temp dir")
local zippath = tmpdir .. "/test.zip"

-- Create test content files
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

-- Cleanup
os.execute("rm -rf " .. tmpdir)

print("PASS")
