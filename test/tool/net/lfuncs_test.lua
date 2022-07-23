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

unix.pledge("stdio")

x = Rdtsc()
y = Rdtsc()
assert(y > x)

assert(Rdrand() ~= Rdrand())
assert(Rdseed() ~= Rdseed())

assert(Bsr(1) == 0)
assert(Bsr(2) == 1)
assert(Bsr(3) == 1)
assert(Bsr(4) == 2)
assert(Bsr(0x80000001) == 31)

assert(Bsf(1) == 0)
assert(Bsf(2) == 1)
assert(Bsf(3) == 0)
assert(Bsf(4) == 2)
assert(Bsf(0x80000001) == 0)

assert(Popcnt(0) == 0)
assert(Popcnt(1) == 1)
assert(Popcnt(2) == 1)
assert(Popcnt(3) == 2)
assert(Popcnt(0b0111101001101001) == 9)

assert(Lemur64() == 0x1940efe9d47ae889)
assert(Lemur64() == 0xd4b3103f567f9974)

assert(hex(0x1940efe9d47ae889) == "0x1940efe9d47ae889")
assert(oct(0x1940efe9d47ae889) == "0145007376472436564211")
assert(bin(0x1940efe9d47ae889) == "0b0001100101000000111011111110100111010100011110101110100010001001")

assert(EscapeHtml("?hello&there<>") == "?hello&amp;there&lt;&gt;")
assert(EscapeParam("?hello&there<>") == "%3Fhello%26there%3C%3E")

assert(DecodeLatin1("hello\xff\xc0") == "helloÿÀ")
assert(EncodeLatin1("helloÿÀ") == "hello\xff\xc0")

url = ParseUrl("https://jart:pass@redbean.dev/2.0.html?x&y=z#frag")
assert(url.scheme == "https")
assert(url.user == "jart")
assert(url.pass == "pass")
assert(url.host == "redbean.dev")
assert(not url.port)
assert(url.path == "/2.0.html")
assert(EncodeLua(url.params) == '{{"x"}, {"y", "z"}}')
assert(url.fragment == "frag")

assert(DecodeBase64("abcdefgABCDE") == "\x69\xb7\x1d\x79\xf8\x00\x04\x20\xc4")
assert(EncodeBase64("\x69\xb7\x1d\x79\xf8\x00\x04\x20\xc4") == "abcdefgABCDE")

assert(Decimate("\xff\xff\x00\x00\xff\xff\x00\x00\xff\xff\x00\x00") == "\xff\x00\xff\x00\xff\x00")

assert(Underlong("hello") == "hello")
assert(Underlong("hello\xc0\x80") == "hello\x00")

assert(ParseIp("x") == -1)
assert(ParseIp("127.0.0.1") == 0x7f000001)

assert(GetMonospaceWidth('c') == 1)
assert(GetMonospaceWidth(string.byte('c')) == 1)
assert(GetMonospaceWidth(0) == 0)
assert(GetMonospaceWidth(1) == -1)
assert(GetMonospaceWidth(32) == 1)
assert(GetMonospaceWidth(0x3061) == 2)
assert(GetMonospaceWidth("ちち") == 4)

assert(IsPublicIp(0x08080808))
assert(not IsPublicIp(0x0a000000))
assert(not IsPublicIp(0x7f000001))

assert(not IsPrivateIp(0x08080808))
assert(IsPrivateIp(0x0a000000))
assert(not IsPrivateIp(0x7f000001))

assert(not IsLoopbackIp(0x08080808))
assert(not IsLoopbackIp(0x0a000000))
assert(IsLoopbackIp(0x7f000001))

assert(IndentLines("hi\nthere") == " hi\n there")
assert(IndentLines("hi\nthere\n") == " hi\n there\n")
assert(IndentLines("hi\nthere\n", 2) == "  hi\n  there\n")

assert(ParseHttpDateTime("Fri, 08 Jul 2022 16:17:43 GMT") == 1657297063)
assert(FormatHttpDateTime(1657297063) == "Fri, 08 Jul 2022 16:17:43 GMT")

assert(VisualizeControlCodes("hello\x00") == "hello␀")
assert(VisualizeControlCodes("\xe2\x80\xa8") == "↵")  -- line separator
assert(VisualizeControlCodes("\xe2\x80\xaa") == "⟫")  -- left-to-right embedding
assert(VisualizeControlCodes("\xe2\x80\xab") == "⟪")  -- right-to-left embedding

assert(math.floor(10 * MeasureEntropy("            ") + .5) == 0)
assert(math.floor(10 * MeasureEntropy("abcabcabcabc") + .5) == 16)

assert(Crc32(0, "123456789") == 0xcbf43926)
assert(Crc32c(0, "123456789") == 0xe3069283)
assert(Md5("hello") == "\x5d\x41\x40\x2a\xbc\x4b\x2a\x76\xb9\x71\x9d\x91\x10\x17\xc5\x92")
assert(Sha1("hello") == "\xaa\xf4\xc6\x1d\xdc\xc5\xe8\xa2\xda\xbe\xde\x0f\x3b\x48\x2c\xd9\xae\xa9\x43\x4d")
assert(Sha224("hello") == "\xea\x09\xae\x9c\xc6\x76\x8c\x50\xfc\xee\x90\x3e\xd0\x54\x55\x6e\x5b\xfc\x83\x47\x90\x7f\x12\x59\x8a\xa2\x41\x93")
assert(Sha256("hello") == "\x2c\xf2\x4d\xba\x5f\xb0\xa3\x0e\x26\xe8\x3b\x2a\xc5\xb9\xe2\x9e\x1b\x16\x1e\x5c\x1f\xa7\x42\x5e\x73\x04\x33\x62\x93\x8b\x98\x24")
assert(Sha384("hello") == "\x59\xe1\x74\x87\x77\x44\x8c\x69\xde\x6b\x80\x0d\x7a\x33\xbb\xfb\x9f\xf1\xb4\x63\xe4\x43\x54\xc3\x55\x3b\xcd\xb9\xc6\x66\xfa\x90\x12\x5a\x3c\x79\xf9\x03\x97\xbd\xf5\xf6\xa1\x3d\xe8\x28\x68\x4f")
assert(Sha512("hello") == "\x9b\x71\xd2\x24\xbd\x62\xf3\x78\x5d\x96\xd4\x6a\xd3\xea\x3d\x73\x31\x9b\xfb\xc2\x89\x0c\xaa\xda\xe2\xdf\xf7\x25\x19\x67\x3c\xa7\x23\x23\xc3\xd9\x9b\xa5\xc1\x1d\x7c\x7a\xcc\x6e\x14\xb8\xc5\xda\x0c\x46\x63\x47\x5c\x2e\x5c\x3a\xde\xf4\x6f\x73\xbc\xde\xc0\x43")

assert(assert(Deflate("hello")) == "\xcbH\xcd\xc9\xc9\x07\x00")
assert(assert(Inflate("\xcbH\xcd\xc9\xc9\x07\x00", 5)) == "hello")

-- deprecated compression api we wish to forget as quickly as possible
assert(Uncompress(Compress("hello")) == "hello")
assert(Compress("hello") == "\x05\x86\xa6\x106x\x9c\xcbH\xcd\xc9\xc9\x07\x00\x06,\x02\x15")
assert(Compress("hello", 0) == "\x05\x86\xa6\x106x\x01\x01\x05\x00\xfa\xffhello\x06,\x02\x15")
assert(Compress("hello", 0, true) == "x\x01\x01\x05\x00\xfa\xffhello\x06,\x02\x15")
