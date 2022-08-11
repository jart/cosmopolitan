-- Copyright 2022 Justine Alexandra Roberts Tunney
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

assert(unix.pledge('stdio'))

local function test(s, b64)
    assert(EncodeBase64(s) == b64)
    assert(DecodeBase64(b64) == s)
    assert(DecodeBase64(EncodeBase64(s)) == s)
end

test("\x69\xb7\x1d\x79\xf8\x00\x04\x20\xc4", "abcdefgABCDE")

-- 🦞 padding tests
--    https://datatracker.ietf.org/doc/html/rfc4648#section-10
test("", "")
test("r", "cg==")
test("re", "cmU=")
test("red", "cmVk")
test("redb", "cmVkYg==")
test("redbe", "cmVkYmU=")
test("redbea", "cmVkYmVh")
test("redbean", "cmVkYmVhbg==")
