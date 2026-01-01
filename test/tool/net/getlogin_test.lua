-- Copyright 2024 Will Maier
--
-- Permission to use, copy, modify, and/or distribute this software for
-- any purpose with or without fee is hereby granted, provided that the
-- above copyright notice and this permission notice appear in all copies.
--
-- THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
-- WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
-- WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
-- AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
-- DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
-- PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
-- TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
-- PERFORMANCE OF THIS SOFTWARE.

assert(unix.pledge("stdio"))

-- Test basic getlogin - should return a string or nil
local login = unix.getlogin()

if login then
  -- If we got a login name, it should be a non-empty string
  assert(type(login) == "string")
  assert(#login > 0)
  print("Login name: " .. login)

  -- On many systems, getlogin() should match LOGNAME or USER env var
  local logname = os.getenv("LOGNAME")
  local user = os.getenv("USER")

  if logname or user then
    -- At least one should match if available
    local matches = (login == logname) or (login == user)
    if not matches then
      print("Warning: getlogin() returned '" .. login ..
            "' but LOGNAME=" .. tostring(logname) ..
            " and USER=" .. tostring(user))
    end
  end
else
  -- getlogin() can fail in some environments (e.g., no controlling terminal)
  print("getlogin() returned nil (this is acceptable in some environments)")
end

print("All getlogin tests passed!")
