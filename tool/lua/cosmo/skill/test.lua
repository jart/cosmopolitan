-- test skill module

local skill = require("cosmo.skill")
local unix = require("cosmo.unix")
local path = require("cosmo.path")

-- Test that module loads
assert(skill, "skill module should load")
assert(type(skill.install) == "function", "skill.install should be a function")

-- Test actual install to temp directory
local tmpdir = unix.mkdtemp(path.join(os.getenv("TMPDIR") or "/tmp", "lua_skill_test_XXXXXX"))
assert(tmpdir, "mkdtemp should create temp directory")
local ok, err = skill.install(tmpdir .. "/")
assert(ok, "skill.install failed: " .. (err or ""))

-- Verify SKILL.md was created
local skill_file = path.join(tmpdir, "cosmo-lua", "SKILL.md")
local f = io.open(skill_file)
assert(f, "SKILL.md should exist at " .. skill_file)
local content = f:read("*a")
f:close()
assert(content:match("^%-%-%-"), "SKILL.md should start with frontmatter")
assert(content:match("name: cosmo%-lua"), "SKILL.md should have correct name")
assert(content:match("whilp/cosmopolitan"), "SKILL.md should reference whilp/cosmopolitan")
assert(content:match("Fetch"), "SKILL.md should document Fetch")
assert(content:match("unix"), "SKILL.md should document unix module")

-- Verify only SKILL.md was created (no other files)
local count = 0
for entry in unix.opendir(path.join(tmpdir, "cosmo-lua")) do
  if entry ~= "." and entry ~= ".." then
    count = count + 1
    assert(entry == "SKILL.md", "only SKILL.md should be created, found: " .. entry)
  end
end
assert(count == 1, "should have exactly one file, got " .. count)

-- Cleanup
unix.rmrf(tmpdir)

print("all skill tests passed")
