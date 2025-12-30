-- skill module for cosmo lua
-- Installs a Claude Code skill

local skill = {}

local SKILL_NAME = "cosmo-lua"

local SKILL_CONTENT = [[---
name: cosmo-lua
description: Use cosmopolitan Lua (cosmo-lua) for portable scripts. Includes HTTP, JSON, unix syscalls, path utils, regex, sqlite, argon2.
allowed-tools: [Read, Write, Edit, Bash, Glob, Grep]
---

# Cosmo Lua

Portable Lua 5.4 with batteries included. Single binary runs on Linux, macOS, Windows, FreeBSD, OpenBSD, NetBSD.

## Installation

```bash
curl -L -o lua https://github.com/whilp/cosmopolitan/releases/latest/download/lua
chmod +x lua
./lua --skill  # install this skill
```

## Getting Help

The executable has built-in documentation. `help` is a global function:

```bash
./lua -e 'help("cosmo.Fetch")'         # look up a function
./lua -e 'help("cosmo.unix")'          # list module functions
./lua -e 'help.search("socket")'       # search by keyword
```

In the REPL:
```lua
help()                -- overview of all modules
help("cosmo.Fetch")   -- function docs
help("cosmo.unix")    -- module functions
```

## Quick Reference

Top-level functions: `local cosmo = require("cosmo")`
Submodules: `local unix = require("cosmo.unix")`

### HTTP & Networking (replaces curl, luasocket)

| Function | Purpose |
|----------|---------|
| `Fetch(url)` | HTTP GET/POST with redirects, TLS, proxy support |
| `ResolveIp(host)` | DNS lookup |
| `ParseUrl(url)` | Parse URL into components |
| `FormatIp(ip)` | Format IP address |
| `IsPublicIp(ip)` | Check if IP is public |

### JSON (replaces dkjson, cjson)

| Function | Purpose |
|----------|---------|
| `DecodeJson(str)` | Parse JSON string to Lua table |
| `EncodeJson(tbl)` | Encode Lua table to JSON string |

### Encoding & Hashing

| Function | Purpose |
|----------|---------|
| `EncodeBase64(s)` | Base64 encode |
| `DecodeBase64(s)` | Base64 decode |
| `EncodeHex(s)` | Hex encode |
| `DecodeHex(s)` | Hex decode |
| `Sha256(s)` | SHA-256 hash |
| `Sha1(s)` | SHA-1 hash |
| `Md5(s)` | MD5 hash |
| `GetRandomBytes(n)` | Cryptographic random bytes |

### Compression

| Function | Purpose |
|----------|---------|
| `Deflate(s)` | Compress with zlib |
| `Inflate(s)` | Decompress zlib |

### Path Utilities: `require("cosmo.path")`

| Function | Purpose |
|----------|---------|
| `path.join(...)` | Join path components |
| `path.basename(p)` | Get filename from path |
| `path.dirname(p)` | Get directory from path |
| `path.exists(p)` | Check if path exists |
| `path.isfile(p)` | Check if path is file |
| `path.isdir(p)` | Check if path is directory |

### POSIX/Unix: `require("cosmo.unix")`

| Function | Purpose |
|----------|---------|
| `unix.open(path, flags, mode)` | Open fd |
| `unix.read(fd)` | Read from fd |
| `unix.write(fd, data)` | Write to fd |
| `unix.close(fd)` | Close fd |
| `unix.chmod(path, mode)` | Change file mode |
| `unix.mkdir(path, mode)` | Create directory |
| `unix.makedirs(path, mode)` | Create directory tree |
| `unix.stat(path)` | Get file metadata |
| `unix.fork()` | Fork process |
| `unix.execve(prog, args)` | Execute program |
| `unix.environ()` | Get environment |
| `unix.getpid()` | Get process ID |
| `unix.sleep(secs)` | Sleep |
| `unix.clock_gettime()` | High-resolution time |

**Octal modes**: Lua has no octal literals. Use `tonumber("755", 8)` for modes.

See `help("unix")` for 100+ additional syscall wrappers.

### Regular Expressions: `require("cosmo.re")`

| Function | Purpose |
|----------|---------|
| `re.search(pattern, str)` | Search for pattern |
| `re.compile(pattern)` | Compile regex for reuse |
| `regex:search(str)` | Search with compiled regex |

### SQLite: `require("cosmo.lsqlite3")`

| Function | Purpose |
|----------|---------|
| `sqlite.open(path)` | Open database |
| `sqlite.open_memory()` | Open in-memory database |
| `db:exec(sql)` | Execute SQL |
| `db:prepare(sql)` | Prepare statement |
| `stmt:step()` | Execute prepared statement |
| `db:close()` | Close database |

### Password Hashing: `require("cosmo.argon2")`

| Function | Purpose |
|----------|---------|
| `argon2.hash_encoded(pw, salt)` | Hash password |
| `argon2.verify(encoded, pw)` | Verify password |

## Example

```lua
local cosmo = require("cosmo")
local path = require("cosmo.path")

-- Fetch JSON from an API
local status, headers, body = cosmo.Fetch("https://api.example.com/data")
if status == 200 then
  local data = cosmo.DecodeJson(body)
  print(data.message)
end

-- Work with files
local configpath = path.join(os.getenv("HOME"), ".config", "app.json")
if path.exists(configpath) then
  local f = io.open(configpath)
  local config = cosmo.DecodeJson(f:read("*a"))
  f:close()
end
```

## More Information

Use `help.search(keyword)` to find functions. The help system has complete documentation for all functions including parameters, return values, and examples.
]]

local unix = require("cosmo.unix")
local path = require("cosmo.path")

local function write_file(filepath, content)
  local dir = path.dirname(filepath)
  if dir and dir ~= "" and dir ~= "." then
    local ok, err = unix.makedirs(dir, tonumber("755", 8))
    if not ok and err:errno() ~= unix.EEXIST then
      return nil, "makedirs failed: " .. tostring(err)
    end
  end
  local fd, err = unix.open(filepath, unix.O_WRONLY | unix.O_CREAT | unix.O_TRUNC, tonumber("644", 8))
  if not fd then
    return nil, "open failed: " .. tostring(err)
  end
  local ok, werr = unix.write(fd, content)
  unix.close(fd)
  if not ok then
    return nil, "write failed: " .. tostring(werr)
  end
  return true
end

local function default_path()
  local home = os.getenv("HOME")
  if not home then
    return nil, "HOME environment variable not set"
  end
  return path.join(home, ".claude", "skills")
end

function skill.install(dest)
  if not dest then
    local default, err = default_path()
    if not default then
      return nil, err
    end
    dest = default
  elseif dest:sub(-1) ~= "/" then
    dest = path.join(dest, ".claude", "skills")
  end

  local skill_dir = path.join(dest, SKILL_NAME)
  local skill_file = path.join(skill_dir, "SKILL.md")

  local ok, err = write_file(skill_file, SKILL_CONTENT)
  if not ok then
    return nil, "failed to write SKILL.md: " .. err
  end

  io.write("installed skill to: " .. skill_dir .. "\n")
  return true
end

return skill
