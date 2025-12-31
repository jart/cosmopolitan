-- cosmo.zip wrapper
-- Provides unified open(path, mode) API with append ("a") support

-- Get the underlying C module
local czip = package.loaded["cosmo.zip.c"] or require("cosmo.zip.c")

-- Appender metatable
local Appender = {}
Appender.__index = Appender
Appender.__name = "zip.Appender"

function Appender:__tostring()
  if self._closed then
    return "zip.Appender (closed)"
  else
    return string.format("zip.Appender (%d pending)", #self._entries)
  end
end

function Appender:add(name, content)
  if self._closed then
    return nil, "appender is closed"
  end
  local ok, err = czip.validate_name(name)
  if not ok then
    return nil, err
  end
  if type(content) ~= "string" then
    return nil, "content must be a string"
  end
  -- Check content size limit
  if self._max_file_size and #content > self._max_file_size then
    return nil, "content exceeds max_file_size limit"
  end
  -- Check for duplicate entry
  if self._known_names[name] then
    return nil, "duplicate entry name"
  end
  self._known_names[name] = true
  table.insert(self._entries, {
    name = name,
    content = content,
  })
  return true
end

function Appender:close()
  if self._closed then
    return true
  end
  self._closed = true

  if #self._entries == 0 then
    return true
  end

  -- fd mode not supported for append (requires reading existing content)
  if self._fd then
    return nil, "append mode with file descriptor is not supported"
  end

  -- Read existing entries from the zip
  local existing_entries = {}
  local reader, err = czip.open(self._path)
  if reader then
    local names = reader:list()
    if names then
      for _, name in ipairs(names) do
        local stat = reader:stat(name)
        local content = reader:read(name)
        if content and stat then
          table.insert(existing_entries, {
            name = name,
            content = content,
            mtime = stat.mtime,
            mode = stat.mode,
            method = stat.method == 0 and "store" or "deflate",
          })
        end
      end
    end
    reader:close()
  end
  -- If we couldn't read the file, it might be empty/new - that's ok

  -- Create new zip with all entries (this truncates the file)
  local writer, err = czip.create(self._path)
  if not writer then
    return nil, "failed to create zip: " .. tostring(err)
  end

  -- Add existing entries first
  for _, entry in ipairs(existing_entries) do
    local ok, err = writer:add(entry.name, entry.content, {
      mtime = entry.mtime,
      mode = entry.mode,
      method = entry.method,
    })
    if not ok then
      writer:close()
      return nil, "failed to re-add existing entry '" .. entry.name .. "': " .. tostring(err)
    end
  end

  -- Add new entries
  for _, entry in ipairs(self._entries) do
    local ok, err = writer:add(entry.name, entry.content)
    if not ok then
      writer:close()
      return nil, "failed to add new entry '" .. entry.name .. "': " .. tostring(err)
    end
  end

  return writer:close()
end

Appender.__close = Appender.close
Appender.__gc = Appender.close

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
    local max_file_size = opts and opts.max_file_size
    if max_file_size ~= nil then
      if type(max_file_size) ~= "number" or max_file_size <= 0 then
        return nil, "max_file_size must be a positive number"
      end
    end
    -- Read existing entry names to check for duplicates
    local known_names = {}
    local reader = czip.open(path_or_fd)
    if reader then
      local names = reader:list()
      if names then
        for _, name in ipairs(names) do
          known_names[name] = true
        end
      end
      reader:close()
    end
    return setmetatable({
      _path = path_or_fd,
      _entries = {},
      _known_names = known_names,
      _max_file_size = max_file_size,
      _closed = false,
    }, Appender)
  else
    return nil, "invalid mode: " .. tostring(mode) .. " (use 'r', 'w', or 'a')"
  end
end

return {
  open = open,
}
