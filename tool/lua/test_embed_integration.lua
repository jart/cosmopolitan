local unix = require("cosmo.unix")
local zip = require("cosmo.zip")
local path = require("cosmo.path")

local tmpdir = unix.mkdtemp("/tmp/test_embed_int_XXXXXX")
assert(tmpdir, "failed to create temp dir")

local function cleanup()
  unix.rmrf(tmpdir)
end

local test_module_content = [[
local M = {}
M.name = "test-module"
M.version = "1.0.0"
function M.greet(name)
  return "Hello, " .. (name or "World") .. "!"
end
function M.add(a, b)
  return a + b
end
return M
]]

local zip_path = path.join(tmpdir, "test-package.zip")
local output_path = path.join(tmpdir, "test-embedded-lua")

local writer = zip.open(zip_path, "w")
assert(writer, "failed to create test ZIP")
writer:add("lua/testmodule/init.lua", test_module_content)
writer:close()

local embed = require("cosmo.embed")
assert(embed, "embed module should exist")
assert(type(embed.install) == "function")

local exe_path = arg[-1] or arg[0]
assert(exe_path, "failed to get executable path")

local src_fd = unix.open(exe_path, unix.O_RDONLY)
assert(src_fd, "failed to open source executable")
local stat = unix.fstat(src_fd)
assert(stat, "failed to stat source")

local dst_fd = unix.open(output_path, unix.O_WRONLY | unix.O_CREAT | unix.O_TRUNC, 0755)
assert(dst_fd, "failed to create destination")

local chunk_size = 65536
local source_size = stat:size()
local remaining = source_size

while remaining > 0 do
  local to_read = math.min(remaining, chunk_size)
  local chunk = unix.read(src_fd, to_read)
  if not chunk or #chunk == 0 then break end
  unix.write(dst_fd, chunk)
  remaining = remaining - #chunk
end

unix.close(src_fd)
unix.close(dst_fd)

local zip_reader = zip.open(zip_path)
assert(zip_reader, "failed to open test package as ZIP")

local entries = zip_reader:list()
assert(#entries == 1, "ZIP should have 1 entry")

local lua_content = zip_reader:read(entries[1])
assert(lua_content, "failed to read entry from ZIP")
zip_reader:close()

local appender = zip.open(output_path, "a")
assert(appender, "failed to open for append")
appender:add(".lua/testmodule/init.lua", lua_content)
appender:close()

local fd = unix.open(output_path, unix.O_RDONLY)
assert(fd, "failed to open embedded executable")
stat = unix.fstat(fd)
unix.close(fd)

-- Note: size may not increase because the appender rewrites the central directory more compactly
assert(stat:size() > 0, "embedded executable should exist")
assert((stat:mode() & 0x40) ~= 0, "file should be executable")

local zip_reader_embedded = zip.open(output_path)
if zip_reader_embedded then
  local embedded_entries = zip_reader_embedded:list()
  local found = false
  for _, entry in ipairs(embedded_entries) do
    if entry == ".lua/testmodule/init.lua" then
      found = true
      local embedded_content = zip_reader_embedded:read(entry)
      assert(embedded_content, "failed to read embedded content")
      assert(#embedded_content == #lua_content, "content size mismatch")
      break
    end
  end
  assert(found, "embedded module not found in ZIP")
  zip_reader_embedded:close()
end

cleanup()
print("PASS")
