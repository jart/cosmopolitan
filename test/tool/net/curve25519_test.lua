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

local secret1 = GetRandomBytes(32)
local public1 = Curve25519(secret1, "\9")
local secret2 = GetRandomBytes(32)
local public2 = Curve25519(secret2, "\9")
local secret3 = GetRandomBytes(32)
local public3 = Curve25519(secret3, "\9")

local shared_key1 = Curve25519(secret1, public2)
local shared_key2 = Curve25519(secret2, public1)

local shared_key3 = Curve25519(secret2, public3)
local shared_key4 = Curve25519(secret3, public2)

assert(shared_key1 == shared_key2)
assert(shared_key1 ~= shared_key3)
assert(shared_key2 ~= shared_key3)
assert(shared_key4 == shared_key3)
