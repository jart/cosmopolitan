-- skill module for cosmo lua
-- Generates and installs a Claude Code skill with documentation

local unix = require("cosmo.unix")
local path = require("cosmo.path")

local skill = {}

local SKILL_NAME = "cosmo-lua"

local SKILL_MD_HEADER = [[---
name: cosmo-lua
description: Use cosmopolitan Lua (cosmo-lua) for portable scripts. Download from whilp/cosmopolitan releases. Includes unix syscalls, path utils, regex, sqlite, argon2.
allowed-tools: [Read, Write, Edit, Bash, Glob, Grep, WebFetch]
---

# Cosmo Lua

Portable Lua with batteries included from [whilp/cosmopolitan](https://github.com/whilp/cosmopolitan).

## Installation

Download the latest `lua` binary from [releases](https://github.com/whilp/cosmopolitan/releases):

```bash
curl -L -o lua https://github.com/whilp/cosmopolitan/releases/latest/download/lua
chmod +x lua
```

The binary runs on Linux, macOS, Windows, FreeBSD, OpenBSD, and NetBSD without dependencies.

## Update

Download the latest release and reinstall the skill:

```bash
curl -L -o lua https://github.com/whilp/cosmopolitan/releases/latest/download/lua
chmod +x lua
./lua --skill
```

## Built-in help

Use the interactive help system:

```lua
local help = require("cosmo.help")
help()                    -- overview
help("cosmo")             -- list module
help("cosmo.Fetch")       -- function docs
help.search("base64")     -- search
```

## Modules

]]

-- Format a documentation entry for markdown
local function format_doc_md(name, doc)
  local lines = {}

  -- Function name as header
  local short_name = name:match("([^%.]+)$") or name
  table.insert(lines, "### " .. short_name)
  table.insert(lines, "")

  -- Signature in code block
  table.insert(lines, "```lua")
  table.insert(lines, doc.signature)
  table.insert(lines, "```")
  table.insert(lines, "")

  -- Description
  if doc.desc and doc.desc ~= "" then
    for desc_line in doc.desc:gmatch("[^\n]+") do
      table.insert(lines, desc_line:match("^%s*(.*)$"))
    end
    table.insert(lines, "")
  end

  -- Parameters
  if #doc.params > 0 then
    table.insert(lines, "**Parameters:**")
    table.insert(lines, "")
    for _, param in ipairs(doc.params) do
      local optional = param.name:match("%?$") and " *(optional)*" or ""
      local clean_name = param.name:gsub("%?$", "")
      if param.desc ~= "" then
        table.insert(lines, string.format("- `%s` (%s)%s: %s", clean_name, param.type, optional, param.desc))
      else
        table.insert(lines, string.format("- `%s` (%s)%s", clean_name, param.type, optional))
      end
    end
    table.insert(lines, "")
  end

  -- Returns
  if #doc.returns > 0 then
    table.insert(lines, "**Returns:**")
    table.insert(lines, "")
    for _, ret in ipairs(doc.returns) do
      if ret.desc ~= "" then
        table.insert(lines, string.format("- `%s`: %s", ret.type, ret.desc))
      else
        table.insert(lines, string.format("- `%s`", ret.type))
      end
    end
    table.insert(lines, "")
  end

  return table.concat(lines, "\n")
end

-- Discover modules from help._docs
-- Returns table of {prefix = {funcs}, ...} where prefix is "" for top-level
local function discover_modules(help_docs)
  local modules = {}

  for name, doc in pairs(help_docs) do
    local prefix = name:match("^([^%.]+)%.") or ""
    if not modules[prefix] then
      modules[prefix] = {}
    end
    table.insert(modules[prefix], {name = name, doc = doc})
  end

  -- Sort functions within each module
  for prefix, funcs in pairs(modules) do
    table.sort(funcs, function(a, b)
      local a_short = a.name:match("([^%.]+)$") or a.name
      local b_short = b.name:match("([^%.]+)$") or b.name
      return a_short < b_short
    end)
  end

  return modules
end

-- Get module display name and file name
local function module_info(prefix)
  if prefix == "" then
    return "cosmo", "cosmo.md"
  else
    return "cosmo." .. prefix, "cosmo-" .. prefix .. ".md"
  end
end

-- Generate markdown for a module
local function generate_module_md(module_name, funcs)
  local lines = {"# " .. module_name, ""}

  for _, item in ipairs(funcs) do
    table.insert(lines, format_doc_md(item.name, item.doc))
  end

  return table.concat(lines, "\n")
end

-- Create directory and parents (like mkdir -p)
local function mkdir_p(dir)
  local is_absolute = dir:sub(1, 1) == "/"
  local parts = {}
  for part in dir:gmatch("[^/]+") do
    table.insert(parts, part)
    local current = (is_absolute and "/" or "") .. table.concat(parts, "/")
    local stat = unix.stat(current)
    if not stat then
      local ok, err = unix.mkdir(current, 0755)
      if not ok and err:errno() ~= unix.EEXIST then
        return nil, "mkdir " .. current .. ": " .. tostring(err)
      end
    end
  end
  return true
end

-- Write a file, creating parent directories as needed
local function write_file(filepath, content)
  local dir = path.dirname(filepath)
  if dir and dir ~= "" and dir ~= "." then
    local ok, err = mkdir_p(dir)
    if not ok then
      return nil, err
    end
  end

  local f = io.open(filepath, "w")
  if not f then
    return nil, "failed to open file: " .. filepath
  end
  f:write(content)
  f:close()
  return true
end

-- Get the default skill installation path
local function default_path()
  local home = os.getenv("HOME")
  if not home then
    return nil, "HOME environment variable not set"
  end
  return path.join(home, ".claude", "skills")
end

-- Generate SKILL.md content with module links (dynamic)
function skill.generate_skill_md(modules)
  local content = SKILL_MD_HEADER

  -- Sort prefixes for consistent output
  local prefixes = {}
  for prefix in pairs(modules) do
    table.insert(prefixes, prefix)
  end
  table.sort(prefixes)

  -- Add module links
  for _, prefix in ipairs(prefixes) do
    local display_name, filename = module_info(prefix)
    local desc = ""
    if prefix == "" then
      desc = "Encoding, hashing, compression, networking"
    elseif prefix == "unix" then
      desc = "POSIX system calls"
    elseif prefix == "path" then
      desc = "Path manipulation"
    elseif prefix == "re" then
      desc = "Regular expressions"
    elseif prefix == "sqlite3" then
      desc = "SQLite database"
    elseif prefix == "argon2" then
      desc = "Password hashing"
    else
      desc = prefix .. " module"
    end
    content = content .. string.format("- [%s](%s) - %s\n", display_name, filename, desc)
  end

  return content
end

-- Generate all reference docs from help system
function skill.generate_docs()
  local help = require("cosmo.help")
  help.load()

  local modules = discover_modules(help._docs)
  local docs = {}

  for prefix, funcs in pairs(modules) do
    local display_name, filename = module_info(prefix)
    docs[filename] = generate_module_md(display_name, funcs)
  end

  return docs, modules
end

-- Install skill to a directory
-- target: target directory (default: ~/.claude/skills)
function skill.install(target)
  if not target then
    local default, err = default_path()
    if not default then
      return nil, err
    end
    target = default
  elseif target:sub(-1) ~= "/" then
    -- If target doesn't end with /, assume it's a project root
    target = path.join(target, ".claude", "skills")
  end

  local skill_dir = path.join(target, SKILL_NAME)

  -- Generate reference docs (also returns discovered modules)
  local docs, modules = skill.generate_docs()

  -- Generate and write SKILL.md
  local skill_content = skill.generate_skill_md(modules)
  local ok, err = write_file(path.join(skill_dir, "SKILL.md"), skill_content)
  if not ok then
    return nil, "failed to write SKILL.md: " .. err
  end

  -- Write reference docs
  for filename, content in pairs(docs) do
    ok, err = write_file(path.join(skill_dir, filename), content)
    if not ok then
      return nil, "failed to write " .. filename .. ": " .. err
    end
  end

  io.write("installed skill to: " .. skill_dir .. "\n")
  return true
end

return skill
