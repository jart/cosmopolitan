-- cosmo.embed.luarocks - LuaRocks package fetching
-- Provides functionality to fetch packages from luarocks.org

local cosmo = require("cosmo")

local LUAROCKS_API = "https://luarocks.org"
local USER_AGENT = "cosmo-lua/1.0"

local function errorf(fmt, ...)
  error(string.format(fmt, ...))
end

local function validate_package_name(name)
  if not name or #name == 0 then
    errorf("Package name cannot be empty")
  end
  if #name > 64 then
    errorf("Package name too long: %s", name)
  end
  if not name:match("^[a-zA-Z][a-zA-Z0-9_%-]*$") then
    errorf("Invalid package name: %s (must be alphanumeric, starting with letter)", name)
  end
  return name
end

local function fetch_url(url)
  local response = cosmo.Fetch(url, {
    headers = {
      ["User-Agent"] = USER_AGENT,
    },
  })
  if response.status ~= 200 then
    errorf("HTTP %d: %s", response.status, url)
  end
  return response.body
end

local function find_author(package_name)
  package_name = validate_package_name(package_name)
  -- LuaRocks.org has no JSON API to look up package owner by name;
  -- the /modules/<pkg> URL redirects to /modules/<author>/<pkg>
  local url = LUAROCKS_API .. "/modules/" .. package_name
  local html = fetch_url(url)
  local author = html:match('/modules/([^/]+)/' .. package_name)
  if not author then
    errorf("Could not determine author for package: %s", package_name)
  end
  return author
end

local function fetch_manifest(author)
  -- Use JSON manifest endpoint for safer parsing (no code execution)
  local url = LUAROCKS_API .. "/manifests/" .. author .. "/manifest.json"
  local content = fetch_url(url)
  local ok, result = pcall(cosmo.DecodeJson, content)
  if not ok then
    errorf("Failed to parse manifest JSON: %s", result)
  end
  return result
end

local function compare_versions(a, b)
  local function parse(v)
    local parts = {}
    for num in v:gmatch("(%d+)") do
      table.insert(parts, tonumber(num))
    end
    return parts
  end
  local pa, pb = parse(a), parse(b)
  for i = 1, math.max(#pa, #pb) do
    local na, nb = pa[i] or 0, pb[i] or 0
    if na ~= nb then
      return na > nb
    end
  end
  return false
end

local function find_package_info(package_name)
  local author = find_author(package_name)
  local manifest = fetch_manifest(author)
  if not manifest.repository or not manifest.repository[package_name] then
    errorf("Package not found in manifest: %s", package_name)
  end
  local versions = {}
  for version in pairs(manifest.repository[package_name]) do
    table.insert(versions, version)
  end
  table.sort(versions, compare_versions)
  if #versions == 0 then
    errorf("No versions found for package: %s", package_name)
  end
  return {
    author = author,
    version = versions[1],
    versions = versions,
  }
end

local function fetch_rockspec(author, package_name, version)
  local url = string.format("%s/manifests/%s/%s-%s.rockspec",
                            LUAROCKS_API, author, package_name, version)
  local content = fetch_url(url)
  local env = {
    package = {},
    version = nil,
    source = {},
    build = {},
    dependencies = {},
    description = {},
  }
  local chunk, err = load(content, "rockspec", "t", env)
  if not chunk then
    errorf("Failed to parse rockspec: %s", err)
  end
  local ok, result = pcall(chunk)
  if not ok then
    errorf("Failed to execute rockspec: %s", result)
  end
  return env
end

local function get_rock_url(author, package_name, version)
  return string.format("%s/manifests/%s/%s-%s.all.rock",
                       LUAROCKS_API, author, package_name, version)
end

local function fetch_rock(author, package_name, version)
  local url = get_rock_url(author, package_name, version)
  return fetch_url(url)
end

return {
  find_package_info = find_package_info,
  fetch_rockspec = fetch_rockspec,
  get_rock_url = get_rock_url,
  fetch_rock = fetch_rock,
  validate_package_name = validate_package_name,
}
