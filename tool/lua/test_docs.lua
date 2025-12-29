-- Tests for documentation accuracy
local cosmo = require("cosmo")
local help = require("cosmo.help")

help.load()

local EXPECTED_MODULES = {"unix", "path", "re", "argon2", "lsqlite3"}

local KNOWN_UNDOCUMENTED = {
  Compress = true, Curve25519 = true, DecodeBase32 = true, EncodeBase32 = true,
  EscapeIp = true, HasControlCodes = true, HighwayHash64 = true,
  IsAcceptableHost = true, IsAcceptablePort = true, IsHeaderRepeatable = true,
  IsValidHttpToken = true, ParseHost = true, ParseParams = true, Uncompress = true,
  ["argon2.hash_len"] = true, ["argon2.m_cost"] = true, ["argon2.parallelism"] = true,
  ["argon2.t_cost"] = true, ["argon2.variant"] = true, ["lsqlite3.config"] = true,
  ["unix.S_ISBLK"] = true, ["unix.S_ISCHR"] = true, ["unix.S_ISDIR"] = true,
  ["unix.S_ISFIFO"] = true, ["unix.S_ISLNK"] = true, ["unix.S_ISREG"] = true,
  ["unix.S_ISSOCK"] = true, ["unix.fstatfs"] = true, ["unix.major"] = true,
  ["unix.minor"] = true, ["unix.setfsgid"] = true, ["unix.sigpending"] = true,
  ["unix.statfs"] = true, ["unix.verynice"] = true,
}

local errors = {}

-- Check submodules are requireable and documented
for _, modname in ipairs(EXPECTED_MODULES) do
  local ok, mod = pcall(require, "cosmo." .. modname)
  if not ok then
    table.insert(errors, "require('cosmo." .. modname .. "') failed")
  else
    -- Check for undocumented functions
    for name, val in pairs(mod) do
      if type(val) == "function" and not name:match("^__") then
        local fullname = modname .. "." .. name
        if not help._docs[fullname] and not KNOWN_UNDOCUMENTED[fullname] then
          table.insert(errors, "undocumented: " .. fullname)
        end
      end
    end
  end
end

-- Check top-level undocumented functions
for name, val in pairs(cosmo) do
  if type(val) == "function" and not help._docs[name] and not KNOWN_UNDOCUMENTED[name] then
    table.insert(errors, "undocumented: " .. name)
  end
end

if #errors > 0 then
  table.sort(errors)
  for _, e in ipairs(errors) do print("FAIL: " .. e) end
  os.exit(1)
end
print("all docs tests passed")
