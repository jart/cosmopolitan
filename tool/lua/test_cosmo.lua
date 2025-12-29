local cosmo = require("cosmo")

assert(type(cosmo) == "table", "cosmo should be a table")

-- test a few top-level functions exist
assert(type(cosmo.DecodeJson) == "function", "DecodeJson should be a function")
assert(type(cosmo.EncodeJson) == "function", "EncodeJson should be a function")
assert(type(cosmo.Sha256) == "function", "Sha256 should be a function")

-- test submodules via require("cosmo.xxx")
local unix = require("cosmo.unix")
local path = require("cosmo.path")
local re = require("cosmo.re")
local argon2 = require("cosmo.argon2")
local sqlite3 = require("cosmo.sqlite3")
assert(type(unix) == "table", "require('cosmo.unix') should return a table")
assert(type(path) == "table", "require('cosmo.path') should return a table")
assert(type(re) == "table", "require('cosmo.re') should return a table")
assert(type(argon2) == "table", "require('cosmo.argon2') should return a table")
assert(type(sqlite3) == "table", "require('cosmo.sqlite3') should return a table")

-- test a function actually works
local json = cosmo.EncodeJson({foo = "bar"})
assert(json == '{"foo":"bar"}', "EncodeJson should produce valid JSON")

local decoded = cosmo.DecodeJson('{"x":1}')
assert(decoded.x == 1, "DecodeJson should parse JSON")

-- test unix.mkdtemp (unix already defined above via require)
local tmpdir = unix.mkdtemp((os.getenv("TMPDIR") or "/tmp") .. "/test_XXXXXX")
assert(tmpdir, "mkdtemp should return a path")
assert(not tmpdir:match("XXXXXX"), "mkdtemp should replace XXXXXX")
local stat = unix.stat(tmpdir)
assert(stat, "mkdtemp should create a directory")
assert(unix.S_ISDIR(stat:mode()), "mkdtemp result should be a directory")
unix.rmdir(tmpdir)

-- test unix.mkstemp
local fd, tmpfile = unix.mkstemp((os.getenv("TMPDIR") or "/tmp") .. "/test_XXXXXX")
assert(fd, "mkstemp should return a file descriptor")
assert(tmpfile, "mkstemp should return a path")
assert(not tmpfile:match("XXXXXX"), "mkstemp should replace XXXXXX")
assert(type(fd) == "number" and fd >= 0, "mkstemp fd should be a non-negative integer")
unix.write(fd, "test")
unix.close(fd)
local stat2 = unix.stat(tmpfile)
assert(stat2, "mkstemp should create a file")
assert(unix.S_ISREG(stat2:mode()), "mkstemp result should be a regular file")
unix.unlink(tmpfile)

print("all tests passed")
