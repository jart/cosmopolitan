local tonumber, tointeger = tonumber, math.tointeger
local type, getmetatable, rawget, error = type, getmetatable, rawget, error
local strsub = string.sub

local print = print

_ENV = nil

-- Try to convert a value to an integer, without assuming any coercion.
local function toint (x)
  x = tonumber(x)   -- handle numerical strings
  if not x then
    return false    -- not coercible to a number
  end
  return tointeger(x)
end


-- If operation fails, maybe second operand has a metamethod that should
-- have been called if not for this string metamethod, so try to
-- call it.
local function trymt (x, y, mtname)
  if type(y) ~= "string" then    -- avoid recalling original metamethod
    local mt = getmetatable(y)
    local mm = mt and rawget(mt, mtname)
    if mm then
      return mm(x, y)
    end
  end
  -- if any test fails, there is no other metamethod to be called
  error("attempt to '" .. strsub(mtname, 3) ..
        "' a " .. type(x) .. " with a " .. type(y), 4)
end


local function checkargs (x, y, mtname)
  local xi = toint(x)
  local yi = toint(y)
  if xi and yi then
    return xi, yi
  else
    return trymt(x, y, mtname), nil
  end
end


local smt = getmetatable("")

smt.__band = function (x, y)
  local x, y = checkargs(x, y, "__band")
  return y and x & y or x
end

smt.__bor = function (x, y)
  local x, y = checkargs(x, y, "__bor")
  return y and x | y or x
end

smt.__bxor = function (x, y)
  local x, y = checkargs(x, y, "__bxor")
  return y and x ~ y or x
end

smt.__shl = function (x, y)
  local x, y = checkargs(x, y, "__shl")
  return y and x << y or x
end

smt.__shr = function (x, y)
  local x, y = checkargs(x, y, "__shr")
  return y and x >> y or x
end

smt.__bnot = function (x)
  local x, y = checkargs(x, x, "__bnot")
  return y and ~x or x
end

