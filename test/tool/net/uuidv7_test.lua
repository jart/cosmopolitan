-- Copyright 2024 Justine Alexandra Roberts Tunney
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
for _ = 1, 1000 do
  local uuid = UuidV7()
  assert(#uuid == 36)
  assert(string.sub(uuid, 9, 9) == "-")
  assert(string.sub(uuid, 14, 14) == "-")
  assert(string.sub(uuid, 15, 15) == "7")
  assert(string.sub(uuid, 19, 19) == "-")
  local y = string.sub(uuid, 20, 20)
  assert(y == "8" or y == "9" or y == "a" or y == "b")
  assert(string.sub(uuid, 24, 24) == "-")
end
