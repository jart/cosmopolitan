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

assert(unix.pledge("stdio"))

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

assert(EncodeHex("\1\2\3\4\255") == "01020304ff")
assert(DecodeHex("01020304ff") == "\1\2\3\4\255")

assert(EncodeBase32("123456") == "64s36d1n6r")
assert(EncodeBase32("12") == "64s0")
assert(EncodeBase32("\33", "01") == "00100001")
assert(EncodeBase32("\33", "0123456789abcdef") == "21")

assert(DecodeBase32("64s36d1n6r") == "123456")
assert(DecodeBase32("64s0") == "12")
assert(DecodeBase32("64 \t\r\ns0") == "12")
assert(DecodeBase32("64s0!64s0") == "12")

assert(EncodeBase32("\1\2\3\4\255", "0123456789abcdef") == "01020304ff")
assert(DecodeBase32("01020304ff", "0123456789abcdef") == "\1\2\3\4\255")

assert(EscapeHtml(nil) == nil)
assert(EscapeHtml("?hello&there<>") == "?hello&amp;there&lt;&gt;")

assert(EscapeParam(nil) == nil)
assert(EscapeParam("?hello&there<>") == "%3Fhello%26there%3C%3E")

assert(DecodeLatin1(nil) == nil)
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

assert(Decimate("\xff\xff\x00\x00\xff\xff\x00\x00\xff\xff\x00\x00") == "\xff\x00\xff\x00\xff\x00")

assert(Underlong(nil) == nil)
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

assert(IndentLines(nil) == nil)
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

assert(assert(Deflate("hello")) == "\xcbH\xcd\xc9\xc9\x07\x00")
assert(assert(Inflate("\xcbH\xcd\xc9\xc9\x07\x00", 5)) == "hello")

-- deprecated compression api we wish to forget as quickly as possible
assert(Uncompress(Compress("hello")) == "hello")
assert(Compress("hello") == "\x05\x86\xa6\x106x\x9c\xcbH\xcd\xc9\xc9\x07\x00\x06,\x02\x15")
assert(Compress("hello", 0) == "\x05\x86\xa6\x106x\x01\x01\x05\x00\xfa\xffhello\x06,\x02\x15")
assert(Compress("hello", 0, true) == "x\x01\x01\x05\x00\xfa\xffhello\x06,\x02\x15")
