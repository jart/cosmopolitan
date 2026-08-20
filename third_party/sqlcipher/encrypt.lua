-- Run: o/tool/net/cherrybean -i tests/encrypt.lua
local sqlite3 = require("lsqlite3")
local enc = os.tmpname()
local plain = os.tmpname()
os.remove(enc)
os.remove(plain)

local db = sqlite3.open(enc)
assert(db:key("correct horse battery staple") == sqlite3.OK, db:errmsg())
assert(db:exec([[CREATE TABLE kv (k TEXT PRIMARY KEY, v TEXT)]]) == sqlite3.OK, db:errmsg())
assert(db:exec([[INSERT INTO kv VALUES ('hello', 'world')]]) == sqlite3.OK, db:errmsg())
db:close()

db = sqlite3.open(enc)
assert(db:key("wrong") == sqlite3.OK)
assert(db:exec("SELECT count(*) FROM kv") ~= sqlite3.OK, "wrong key should not read")
db:close()

db = sqlite3.open(enc)
assert(db:key("correct horse battery staple") == sqlite3.OK, db:errmsg())
local saw = false
for row in db:nrows("SELECT * FROM kv") do
  assert(row.k == "hello" and row.v == "world")
  saw = true
end
assert(saw, "missing row")
assert(db:exec(string.format("ATTACH DATABASE %q AS plain KEY ''", plain)) == sqlite3.OK, db:errmsg())
assert(db:exec("SELECT sqlcipher_export('plain')") == sqlite3.OK, db:errmsg())
db:exec("DETACH DATABASE plain")
db:close()

local f = assert(io.open(enc, "rb"))
local hdr = f:read(16)
f:close()
assert(hdr ~= "SQLite format 3\0", "encrypted file should not be plaintext sqlite")

print("ok", sqlite3.version())
os.remove(enc)
os.remove(plain)
os.exit(0)
