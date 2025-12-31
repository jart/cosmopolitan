-- cosmo.embed - Embed pure Lua libraries into the Lua executable
-- Copyright 2025 Justine Alexandra Roberts Tunney
-- SPDX-License-Identifier: ISC

local cosmo = require("cosmo")
local unix = require("cosmo.unix")
local zip = require("cosmo.zip")
local luarocks = require("cosmo.embed.luarocks")

local function log(msg)
  io.stderr:write(msg .. "\n")
  io.stderr:flush()
end

local function errorf(fmt, ...)
  error(string.format(fmt, ...))
end

local function get_executable_path()
  return arg[-1] or arg[0] or error("Cannot determine executable path")
end

local function is_safe_path(filepath)
  -- Prevent directory traversal - path must stay within target directory
  if filepath:match("%.%.") then
    return false, "path traversal not allowed"
  end
  if filepath:match("^/") then
    return false, "absolute path not allowed"
  end
  return true
end

local function extract_lua_files_from_zip(zip_content)
  local reader, err = zip.from(zip_content)
  if not reader then
    errorf("Failed to open ZIP: %s", err)
  end
  local files = {}
  local entries = reader:list()
  for _, entry in ipairs(entries) do
    if entry:match("%.lua$") and not entry:match("/$") then
      local safe, reason = is_safe_path(entry)
      if not safe then
        log("WARNING: Skipping unsafe path '" .. entry .. "': " .. reason)
      else
        local content, read_err = reader:read(entry)
        if content then
          files[entry] = content
        end
      end
    end
  end
  reader:close()
  return files
end

local function normalize_paths(files, package_name)
  local normalized = {}
  for filepath, content in pairs(files) do
    local zippath = filepath
    zippath = zippath:gsub("^[^/]+/lua/", "")
    zippath = zippath:gsub("^lua/", "")
    if not zippath:match("^" .. package_name .. "/") and
       not zippath:match("^" .. package_name .. "%.lua$") then
      zippath = package_name .. "/" .. zippath
    end
    if not zippath:match("^%.lua/") then
      zippath = ".lua/" .. zippath
    end
    -- Final safety check on normalized path
    local safe, reason = is_safe_path(zippath:gsub("^%.lua/", ""))
    if not safe then
      log("WARNING: Skipping unsafe normalized path '" .. zippath .. "': " .. reason)
    else
      normalized[zippath] = content
    end
  end
  return normalized
end

local function validate_output_path(output_path)
  local safe, reason = is_safe_path(output_path)
  if not safe then
    errorf("%s: %s", reason, output_path)
  end
  return output_path
end

local function copy_executable(src_path, dest_path)
  dest_path = validate_output_path(dest_path)
  local content, err = cosmo.Slurp(src_path)
  if not content then
    errorf("Failed to read source: %s", err or src_path)
  end
  local ok, write_err = cosmo.Barf(dest_path, content, 0755)
  if not ok then
    errorf("Failed to write destination: %s", write_err or dest_path)
  end
end

local function install(package_name, output_path)
  output_path = output_path or ("lua-with-" .. package_name)
  log("Installing package: " .. package_name)

  log("Finding package info...")
  local ok, info = pcall(luarocks.find_package_info, package_name)
  if not ok then
    errorf("Package not found: %s", info)
  end
  log("Found: " .. info.author .. "/" .. package_name .. " v" .. info.version)

  log("Downloading package...")
  local ok2, package_content = pcall(luarocks.fetch_rock, info.author, package_name, info.version)
  if not ok2 then
    errorf("Failed to download: %s", package_content)
  end
  log("Downloaded " .. #package_content .. " bytes")

  log("Extracting Lua files...")
  local files = extract_lua_files_from_zip(package_content)
  local file_count = 0
  for _ in pairs(files) do file_count = file_count + 1 end
  log("Found " .. file_count .. " Lua files")

  if file_count == 0 then
    errorf("No Lua files found in package")
  end

  log("Normalizing paths...")
  local normalized = normalize_paths(files, package_name)

  log("Will embed:")
  for zippath in pairs(normalized) do
    log("  " .. zippath)
  end

  local exe_path = get_executable_path()

  log("Copying executable to: " .. output_path)
  copy_executable(exe_path, output_path)

  log("Embedding files...")
  local appender, err = zip.open(output_path, "a")
  if not appender then
    unix.unlink(output_path)
    errorf("Failed to open for append: %s", err)
  end
  for zippath, content in pairs(normalized) do
    local ok, add_err = appender:add(zippath, content)
    if not ok then
      appender:close()
      unix.unlink(output_path)
      errorf("Failed to add %s: %s", zippath, add_err)
    end
  end
  local ok, close_err = appender:close()
  if not ok then
    unix.unlink(output_path)
    errorf("Failed to close appender: %s", close_err)
  end

  log("Success! Created: " .. output_path)
  return true
end

return {
  install = install,
  is_safe_path = is_safe_path,
  validate_output_path = validate_output_path,
}
