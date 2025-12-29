-- help module for cosmo lua
-- Parses LuaLS-style definitions.lua and provides interactive help()

local help = {
  _docs = {},           -- name -> {desc, params, returns, signature}
  _funcs = {},          -- function reference -> name
  _loaded = false,
}

-- Parse a definitions.lua file and extract documentation
local function parse_definitions(content)
  local docs = {}
  local current_desc = {}
  local current_params = {}
  local current_returns = {}
  local in_multiline_comment = false
  local current_table = nil  -- tracks table context like "unix"
  local current_type = nil   -- tracks @type annotation

  for line in content:gmatch("[^\n]+") do
    -- Skip the meta marker and error line
    if line:match("^%-%-%-@meta") or line:match("^error%(") then
      goto continue
    end

    -- Handle multiline comment blocks --[[ ... ]]
    if line:match("^%-%-%[%[") then
      in_multiline_comment = true
      goto continue
    end
    if line:match("^%]%]") then
      in_multiline_comment = false
      goto continue
    end
    if in_multiline_comment then
      goto continue
    end

    -- Track table context (e.g., "unix = {")
    local table_name = line:match("^([%w_]+)%s*=%s*{")
    if table_name then
      current_table = table_name
      goto continue
    end

    -- End of table
    if line:match("^}") and current_table then
      current_table = nil
      goto continue
    end

    -- Parse @type annotations for constants (may be indented inside tables)
    local type_desc = line:match("^%s*%-%-%-%s*@type%s+%S+%s*(.*)")
    if type_desc then
      current_type = type_desc ~= "" and type_desc or nil
      goto continue
    end

    -- Description line (--- comment without @, handle indented too)
    local desc_text = line:match("^%s*%-%-%-(.*)$")
    if desc_text and not desc_text:match("^%s*@") then
      table.insert(current_desc, desc_text)
      goto continue
    end

    -- @param annotation
    local param_name, param_type, param_desc = line:match("^%s*%-%-%-@param%s+([%w_]+%??)%s+([^%s]+)%s*(.*)")
    if param_name then
      table.insert(current_params, {
        name = param_name,
        type = param_type,
        desc = param_desc or ""
      })
      goto continue
    end

    -- @return annotation
    local ret_type, ret_desc = line:match("^%s*%-%-%-@return%s+([^%s]+)%s*(.*)")
    if ret_type then
      table.insert(current_returns, {
        type = ret_type,
        desc = ret_desc or ""
      })
      goto continue
    end

    -- Skip other annotations (@nodiscard, @class, @overload, etc)
    if line:match("^%s*%-%-%-@") then
      goto continue
    end

    -- Function declaration
    local func_name = line:match("^function%s+([%w_%.]+)%s*%(")
    if func_name then
      -- Extract parameter list from the line
      local params_str = line:match("^function%s+[%w_%.]+%s*%(([^)]*)%)")

      -- Build the documentation entry
      docs[func_name] = {
        desc = table.concat(current_desc, "\n"),
        params = current_params,
        returns = current_returns,
        signature = func_name .. "(" .. (params_str or "") .. ")"
      }

      -- Reset for next function
      current_desc = {}
      current_params = {}
      current_returns = {}
      current_type = nil
    end

    -- Variable/constant declaration (for things like unix.O_RDONLY)
    local var_name = line:match("^([%w_%.]+)%s*=")
    if var_name and #current_desc > 0 then
      docs[var_name] = {
        desc = table.concat(current_desc, "\n"),
        params = {},
        returns = {},
        signature = var_name
      }
      current_desc = {}
      current_params = {}
      current_returns = {}
      current_type = nil
    end

    -- Parse constants inside tables (e.g., "    EEXIST = nil,")
    if current_table then
      local const_name = line:match("^%s+([A-Z][A-Z0-9_]*)%s*=")
      if const_name and (current_type or #current_desc > 0) then
        local full_name = current_table .. "." .. const_name
        local desc = current_type or ""
        if #current_desc > 0 then
          if desc ~= "" then desc = desc .. "\n" end
          desc = desc .. table.concat(current_desc, "\n")
        end
        docs[full_name] = {
          desc = desc,
          params = {},
          returns = {},
          signature = full_name .. " (constant)"
        }
        current_desc = {}
        current_params = {}
        current_returns = {}
        current_type = nil
      end
    end

    ::continue::
  end

  return docs
end

-- Format a documentation entry for display
local function format_doc(name, doc)
  local lines = {}

  -- Signature
  table.insert(lines, doc.signature)
  table.insert(lines, "")

  -- Description
  if doc.desc and doc.desc ~= "" then
    -- Clean up description - remove leading spaces from each line
    for desc_line in doc.desc:gmatch("[^\n]+") do
      table.insert(lines, desc_line:match("^%s*(.*)$"))
    end
    table.insert(lines, "")
  end

  -- Parameters
  if #doc.params > 0 then
    table.insert(lines, "Parameters:")
    for _, param in ipairs(doc.params) do
      local optional = param.name:match("%?$") and " (optional)" or ""
      local clean_name = param.name:gsub("%?$", "")
      if param.desc ~= "" then
        table.insert(lines, string.format("  %s (%s)%s: %s", clean_name, param.type, optional, param.desc))
      else
        table.insert(lines, string.format("  %s (%s)%s", clean_name, param.type, optional))
      end
    end
    table.insert(lines, "")
  end

  -- Returns
  if #doc.returns > 0 then
    table.insert(lines, "Returns:")
    for _, ret in ipairs(doc.returns) do
      if ret.desc ~= "" then
        table.insert(lines, string.format("  %s: %s", ret.type, ret.desc))
      else
        table.insert(lines, string.format("  %s", ret.type))
      end
    end
    table.insert(lines, "")
  end

  return table.concat(lines, "\n")
end

-- Load and parse a definitions file by module name
local function load_defs(modname)
  local path = package.searchpath(modname, package.path)
  if not path then return nil end
  local f = io.open(path, "r")
  if not f then return nil end
  local content = f:read("*a")
  f:close()
  return parse_definitions(content)
end

-- Load definitions
local function load_definitions()
  if help._loaded then return end
  help._docs = load_defs("definitions") or {}
  help._loaded = true
end

-- Public function to ensure definitions are loaded (for programmatic access)
function help.load()
  load_definitions()
end

-- Register function->name mappings for help(func) support
function help.register(tbl, prefix)
  if type(tbl) ~= "table" then return end

  for name, val in pairs(tbl) do
    if type(val) == "function" then
      local fullname = prefix .. "." .. name
      help._funcs[val] = fullname
    elseif type(val) == "table" and name ~= "_G" and not name:match("^_") then
      help.register(val, prefix .. "." .. name)
    end
  end
end

-- List all documented items in a module
local function list_module(prefix)
  load_definitions()

  local functions = {}
  local constants = {}
  local submodules = {}

  for name, doc in pairs(help._docs) do
    local is_const = doc.signature:match("%(constant%)$")
    if name:match("^" .. prefix:gsub("%.", "%%.") .. "%.([^%.]+)$") then
      local short = name:match("([^%.]+)$")
      if is_const then
        table.insert(constants, {name = short, signature = doc.signature})
      else
        table.insert(functions, {name = short, signature = doc.signature})
      end
    elseif name:match("^" .. prefix:gsub("%.", "%%.") .. "%.([^%.]+)%.") then
      local submod = name:match("^" .. prefix:gsub("%.", "%%.") .. "%.([^%.]+)")
      submodules[submod] = true
    end
  end

  table.sort(functions, function(a, b) return a.name < b.name end)
  table.sort(constants, function(a, b) return a.name < b.name end)

  local lines = {prefix, ""}

  -- Show submodules first
  local submods = {}
  for submod in pairs(submodules) do
    table.insert(submods, submod)
  end
  if #submods > 0 then
    table.sort(submods)
    table.insert(lines, "Submodules:")
    for _, submod in ipairs(submods) do
      table.insert(lines, "  " .. prefix .. "." .. submod)
    end
    table.insert(lines, "")
  end

  -- Show functions
  if #functions > 0 then
    table.insert(lines, "Functions:")
    for _, item in ipairs(functions) do
      table.insert(lines, "  " .. item.signature)
    end
    table.insert(lines, "")
  end

  -- Show constants
  if #constants > 0 then
    table.insert(lines, "Constants:")
    for _, item in ipairs(constants) do
      table.insert(lines, "  " .. item.name)
    end
  end

  return table.concat(lines, "\n")
end

-- Main help function
function help.show(what)
  load_definitions()

  -- No argument: show overview
  if what == nil then
    local overview = [[
Cosmo Lua Help System

Modules:
  cosmo         - Encoding, hashing, compression, networking
  cosmo.unix    - POSIX system calls
  cosmo.path    - Path manipulation
  cosmo.re      - Regular expressions
  cosmo.sqlite3 - SQLite database
  cosmo.argon2  - Password hashing

Usage:
  help("cosmo")              - List module contents
  help("cosmo.Fetch")        - Show function documentation
  help(cosmo.Fetch)          - Same, using function reference
  help.search("base64")      - Search for matching functions
]]
    print(overview)
    return
  end

  local name

  -- Function reference
  if type(what) == "function" then
    name = help._funcs[what]
    if not name then
      print("No documentation found (function not registered)")
      return
    end
  -- String name
  elseif type(what) == "string" then
    name = what
  -- Table (module)
  elseif type(what) == "table" then
    -- Try to identify the module by checking a known function
    for func, fname in pairs(help._funcs) do
      local mod_name, func_name = fname:match("^(.+)%.([^%.]+)$")
      if mod_name and what[func_name] == func then
        print(list_module(mod_name))
        return
      end
    end
    print("No documentation found for this table")
    return
  else
    print("Usage: help(name) or help('module.function')")
    return
  end

  -- Look up with fallback: cosmo.X -> X, cosmo.unix.X -> unix.X
  local function find_doc(n)
    if help._docs[n] then return help._docs[n], n end
    -- Try stripping cosmo. prefix
    local stripped = n:match("^cosmo%.(.+)$")
    if stripped and help._docs[stripped] then
      return help._docs[stripped], stripped
    end
    return nil
  end

  local doc, found_name = find_doc(name)
  if doc then
    print(format_doc(found_name, doc))
    return
  end

  -- Check if it's a module prefix (try both with and without cosmo.)
  local function is_module_prefix(prefix)
    local pattern = "^" .. prefix:gsub("%.", "%%.") .. "%."
    for dname in pairs(help._docs) do
      if dname:match(pattern) then return true end
    end
    return false
  end

  if is_module_prefix(name) then
    print(list_module(name))
    return
  end
  -- Try without cosmo. prefix
  local stripped = name:match("^cosmo%.(.+)$")
  if stripped and is_module_prefix(stripped) then
    print(list_module(name))  -- Keep original name for display
    return
  end

  print("No documentation found for: " .. tostring(name))
end

-- Search for functions matching a pattern
function help.search(pattern)
  load_definitions()

  pattern = pattern:lower()
  local matches = {}

  for name, doc in pairs(help._docs) do
    if name:lower():match(pattern) or doc.desc:lower():match(pattern) then
      table.insert(matches, name)
    end
  end

  table.sort(matches)

  if #matches == 0 then
    print("No matches for: " .. pattern)
  else
    print("Matches for '" .. pattern .. "':")
    for _, name in ipairs(matches) do
      print("  " .. name)
    end
  end
end

-- Make help callable directly
setmetatable(help, {
  __call = function(_, what)
    return help.show(what)
  end
})

-- Auto-register cosmo module and submodules when help is loaded
local ok, cosmo = pcall(require, "cosmo")
if ok and cosmo then
  help.register(cosmo, "cosmo")
end

-- Register submodules (they're separate requires now)
for _, submod in ipairs({"unix", "path", "re", "argon2", "lsqlite3"}) do
  local ok, mod = pcall(require, "cosmo." .. submod)
  if ok and mod then
    help.register(mod, "cosmo." .. submod)
  end
end

return help
