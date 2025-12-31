-- cosmo.zip wrapper
-- Provides unified open(path, mode) API

-- Get the underlying C module
local czip = package.loaded["cosmo.zip.c"] or require("cosmo.zip.c")

-- Unified open API
-- path_or_fd can be a string (file path) or integer (file descriptor)
-- Signatures:
--   open(path_or_fd) -> reader (default read mode)
--   open(path_or_fd, opts) -> reader with options (backwards compatible)
--   open(path_or_fd, mode) -> reader/writer/appender based on mode
--   open(path_or_fd, mode, opts) -> with options
local function open(path_or_fd, mode_or_opts, opts)
  local mode = "r"
  if type(mode_or_opts) == "table" then
    opts = mode_or_opts
  elseif type(mode_or_opts) == "string" then
    mode = mode_or_opts
  end

  if mode == "r" then
    return czip.open(path_or_fd, opts)
  elseif mode == "w" then
    return czip.create(path_or_fd, opts)
  elseif mode == "a" then
    if type(path_or_fd) == "number" then
      return nil, "append mode with file descriptor is not supported"
    end
    return czip.append(path_or_fd, opts)
  else
    return nil, "invalid mode: " .. tostring(mode) .. " (use 'r', 'w', or 'a')"
  end
end

-- Create reader from in-memory zip data
-- from(data, [opts]) -> reader | nil, error
local function from(data, opts)
  return czip.from(data, opts)
end

return {
  open = open,
  from = from,
}
