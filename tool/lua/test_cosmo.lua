local cosmo = require("cosmo")

assert(type(cosmo) == "table", "cosmo should be a table")

-- test a few top-level functions exist
assert(type(cosmo.DecodeJson) == "function", "DecodeJson should be a function")
assert(type(cosmo.EncodeJson) == "function", "EncodeJson should be a function")
assert(type(cosmo.Sha256) == "function", "Sha256 should be a function")

-- test submodules exist
assert(type(cosmo.unix) == "table", "cosmo.unix should be a table")
assert(type(cosmo.path) == "table", "cosmo.path should be a table")
assert(type(cosmo.re) == "table", "cosmo.re should be a table")
assert(type(cosmo.argon2) == "table", "cosmo.argon2 should be a table")
assert(type(cosmo.sqlite3) == "table", "cosmo.sqlite3 should be a table")

-- test a function actually works
local json = cosmo.EncodeJson({foo = "bar"})
assert(json == '{"foo":"bar"}', "EncodeJson should produce valid JSON")

local decoded = cosmo.DecodeJson('{"x":1}')
assert(decoded.x == 1, "DecodeJson should parse JSON")

print("all tests passed")
