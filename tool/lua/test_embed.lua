local unix = require("cosmo.unix")
local zip = require("cosmo.zip")
local cosmo = require("cosmo")

local tmpdir = unix.mkdtemp("/tmp/test_embed_XXXXXX")
assert(tmpdir, "failed to create temp dir")

local function cleanup()
  unix.rmrf(tmpdir)
end

local embed = require("cosmo.embed")
assert(embed, "embed module should exist")
assert(type(embed) == "table", "embed module should be a table")
assert(type(embed.install) == "function", "embed.install should be a function")
assert(type(embed.is_safe_path) == "function", "embed.is_safe_path should be a function")
assert(type(embed.validate_output_path) == "function", "embed.validate_output_path should be a function")

-- Test path safety validation (security)
assert(embed.is_safe_path("foo/bar.lua") == true)
assert(embed.is_safe_path("module/init.lua") == true)
assert(embed.is_safe_path("penlight/path.lua") == true)

-- Test unsafe paths are rejected
local safe, reason = embed.is_safe_path("../escape.lua")
assert(not safe, "path traversal should be rejected")

safe, reason = embed.is_safe_path("foo/../bar.lua")
assert(not safe, "embedded path traversal should be rejected")

safe, reason = embed.is_safe_path("/etc/passwd")
assert(not safe, "absolute path should be rejected")

-- Test output path validation (security)
assert(embed.validate_output_path("lua-with-luaunit") == "lua-with-luaunit")
assert(embed.validate_output_path("./output") == "./output")
assert(embed.validate_output_path("subdir/output") == "subdir/output")

-- Test paths that escape current directory are rejected
local function expect_error(fn, ...)
  local ok, err = pcall(fn, ...)
  assert(not ok, "expected error for: " .. tostring(select(1, ...)))
  return err
end

expect_error(embed.validate_output_path, "/absolute/path")
expect_error(embed.validate_output_path, "../escape")
expect_error(embed.validate_output_path, "foo/../bar")

local luarocks = require("cosmo.embed.luarocks")
assert(luarocks, "luarocks module should exist")
assert(type(luarocks.find_package_info) == "function")
assert(type(luarocks.fetch_rockspec) == "function")
assert(type(luarocks.get_rock_url) == "function")
assert(type(luarocks.fetch_rock) == "function")
assert(type(luarocks.validate_package_name) == "function")

-- Test package name validation (security)
assert(luarocks.validate_package_name("luaunit") == "luaunit")
assert(luarocks.validate_package_name("lua-cjson") == "lua-cjson")
assert(luarocks.validate_package_name("penlight") == "penlight")
assert(luarocks.validate_package_name("inspect") == "inspect")

-- Test invalid package names are rejected
expect_error(luarocks.validate_package_name, "")
expect_error(luarocks.validate_package_name, nil)
expect_error(luarocks.validate_package_name, "../escape")
expect_error(luarocks.validate_package_name, "foo/bar")
expect_error(luarocks.validate_package_name, "foo?bar")
expect_error(luarocks.validate_package_name, "1invalid")
expect_error(luarocks.validate_package_name, string.rep("a", 100))

local rockspec_content = [[
package = "testpkg"
version = "1.0-1"
source = {
  url = "https://example.com/testpkg-1.0.zip"
}
description = {
  summary = "Test package",
  license = "MIT"
}
dependencies = {
  "lua >= 5.1"
}
build = {
  type = "builtin",
  modules = {
    testpkg = "testpkg.lua"
  }
}
]]

local env = {
  package = {},
  version = nil,
  source = {},
  build = {},
  dependencies = {},
  description = {},
}

local chunk, err = load(rockspec_content, "rockspec", "t", env)
assert(chunk, "rockspec should parse: " .. tostring(err))

local ok, result = pcall(chunk)
assert(ok, "rockspec should execute: " .. tostring(result))
assert(env.package == "testpkg", "package name should be parsed")
assert(env.version == "1.0-1", "version should be parsed")
assert(env.source.url == "https://example.com/testpkg-1.0.zip", "source URL should be parsed")

cleanup()
print("PASS")
