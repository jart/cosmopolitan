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

assert(EncodeLua(DecodeJson[[     ]]) == 'nil')
assert(EncodeLua(DecodeJson[[  0  ]]) ==  '0' )
assert(EncodeLua(DecodeJson[[ [1] ]]) == '{1}')
assert(EncodeLua(DecodeJson[[ 2.3 ]]) == '2.3')
assert(EncodeLua(DecodeJson[[ [1,3,2] ]]) == '{1, 3, 2}')
-- assert(EncodeLua(DecodeJson[[ {1: 2, 3: 4} ]]) == '{[1]=2, [3]=4}')
assert(EncodeLua(DecodeJson[[ {"foo": 2, "bar": 4} ]]) == '{bar=4, foo=2}')
assert(EncodeLua(DecodeJson[[ null ]]) == 'nil')
assert(EncodeLua(DecodeJson[[ -123 ]]) == '-123')
assert(EncodeLua(DecodeJson[[ 1e6 ]]) == '1000000.')
assert(EncodeLua(DecodeJson[[ 1.e-6 ]]) == '0.000001')
assert(EncodeLua(DecodeJson[[ 1e-06 ]]) == '0.000001')
assert(EncodeLua(DecodeJson[[ 9.123e6 ]]) == '9123000.')
assert(EncodeLua(DecodeJson[[ [{"heh": [1,3,2]}] ]]) == '{{heh={1, 3, 2}}}')
assert(EncodeLua(DecodeJson[[ 3.14159 ]]) == '3.14159')
-- assert(EncodeLua(DecodeJson[[ {3=4} ]]) == '{[3]=4}')
assert(EncodeLua(DecodeJson[[ 1e-12 ]]) == '1e-12')

assert(EncodeJson(DecodeJson[[ 1e-12 ]]) == '1e-12')
assert(EncodeJson(DecodeJson[[ true ]]) == 'true')
assert(EncodeJson(DecodeJson[[ false ]]) == 'false')
assert(EncodeJson(DecodeJson[[ null ]]) == 'null')
assert(EncodeJson(DecodeJson[[ [] ]]) == '[]')
assert(EncodeJson(DecodeJson[[ {} ]]) == '{}')

assert(DecodeJson[["\f"]] == '\f')                        -- c0
assert(DecodeJson[["\t"]] == '\t')                        -- c0
assert(DecodeJson[["\n"]] == '\n')                        -- c0
assert(DecodeJson[["\r"]] == '\r')                        -- c0
assert(DecodeJson[["\\"]] == '\\')                        -- c0
assert(DecodeJson[["\""]] == '\"')                        -- c0
assert(DecodeJson[["\u0100"]] == 'Ā')                     -- latin-1
assert(DecodeJson[["\ud800\udf30\ud800\udf30"]] == '𐌰𐌰')  -- utf-16 astral planes gothic
assert(DecodeJson[["\uD800"]] == '\\uD800')               -- utf-16 invalid (keep utf-8 well-formed)

assert(EncodeJson(DecodeJson[[ -9223372036854775808  ]]) == '-9223372036854775808')  -- minimum 64-bit integer
assert(EncodeJson(DecodeJson[[  9223372036854775807  ]]) ==  '9223372036854775807')  -- maximum 64-bit integer
assert(EncodeJson(DecodeJson[[  9223372036854775808  ]]) ==  '9223372036854776000')  -- switches to double due to integer overflow
assert(EncodeJson(DecodeJson[[ -9223372036854775809  ]]) == '-9223372036854776000')  -- switches to double due to integer underflow
assert(EncodeJson(DecodeJson[[ 9223372036854775807.0 ]]) ==  '9223372036854776000')  -- switches to double due to period mark
assert(EncodeJson(DecodeJson[[ 2.7182818284590452354 ]]) == '2.718281828459045')     -- euler constant w/ 17 digit precision
assert( EncodeLua(DecodeJson[[ 2.7182818284590452354 ]]) == '2.718281828459045')     -- euler constant w/ 17 digit precision

--------------------------------------------------------------------------------
-- benchmark		nanos	ticks
--------------------------------------------------------------------------------
-- JsonParseEmpty	23	72
-- JsonParseInteger	45	142
-- JsonParseDouble	108	335
-- JsonParseString	106	329
-- JsonParseArray	243	754
-- JsonParseObject	523	1622

function JsonParseEmpty()
   DecodeJson[[]]
end

function JsonParseInteger()
   DecodeJson[[ -9223372036854775808 ]]
end

function JsonParseDouble()
   DecodeJson[[ 2.7182818284590452354 ]]
end

function JsonParseString()
   DecodeJson[[ "\ud800\udf30 he𐌰𐌰o \ud800\udf30" ]]
end

function JsonParseArray()
   DecodeJson[[ [123,456,789] ]]
end

function JsonParseObject()
   DecodeJson[[ {"3":"1", "4":"1", "5":"9"} ]]
end

print('JsonParseEmpty', Benchmark(JsonParseEmpty))
print('JsonParseInteg', Benchmark(JsonParseInteger))
print('JsonParseDouble', Benchmark(JsonParseDouble))
print('JsonParseString', Benchmark(JsonParseString))
print('JsonParseArray', Benchmark(JsonParseArray))
print('JsonParseObject', Benchmark(JsonParseObject))
