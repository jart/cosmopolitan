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

assert(EncodeLua(assert(DecodeJson[[  0  ]])) ==  '0' )
assert(EncodeLua(assert(DecodeJson[[ [1] ]])) == '{1}')
assert(EncodeLua(assert(DecodeJson[[ 2.3 ]])) == '2.3')
assert(EncodeLua(assert(DecodeJson[[ [1,3,2] ]])) == '{1, 3, 2}')
assert(EncodeLua(assert(DecodeJson[[ {"foo": 2, "bar": 4} ]])) == '{bar=4, foo=2}')
assert(EncodeLua(assert(DecodeJson[[ -123 ]])) == '-123')
assert(EncodeLua(assert(DecodeJson[[ 1e6 ]])) == '1000000.')
assert(EncodeLua(assert(DecodeJson[[ 1e-6 ]])) == '0.000001')
assert(EncodeLua(assert(DecodeJson[[ 1e-06 ]])) == '0.000001')
assert(EncodeLua(assert(DecodeJson[[ 9.123e6 ]])) == '9123000.')
assert(EncodeLua(assert(DecodeJson[[ [{"heh": [1,3,2]}] ]])) == '{{heh={1, 3, 2}}}')
assert(EncodeLua(assert(DecodeJson[[ 3.14159 ]])) == '3.14159')
assert(EncodeLua(assert(DecodeJson[[ 1e-12 ]])) == '1e-12')
assert(assert(DecodeJson[[ "\u007f" ]]) == '\x7f')
assert(assert(DecodeJson[[ "𝐀 𝐁 𝐂" ]]) == "𝐀 𝐁 𝐂")
assert(assert(DecodeJson[[ "😀 😁 😂" ]]) == "😀 😁 😂")

assert(EncodeJson(assert(DecodeJson[[ 1e-12 ]])) == '1e-12')
assert(EncodeJson(assert(DecodeJson[[ true ]])) == 'true')
assert(EncodeJson(assert(DecodeJson[[ [] ]])) == '[]')
assert(EncodeJson(assert(DecodeJson[[ {} ]])) == '{}')

for c = 0, 127 do
  assert(DecodeJson(assert(EncodeJson(string.char(c)))) == string.char(c))
end

assert(assert(DecodeJson[["\f"]]) == '\f')                        -- c0
assert(assert(DecodeJson[["\t"]]) == '\t')                        -- c0
assert(assert(DecodeJson[["\n"]]) == '\n')                        -- c0
assert(assert(DecodeJson[["\r"]]) == '\r')                        -- c0
assert(assert(DecodeJson[["\\"]]) == '\\')                        -- c0
assert(assert(DecodeJson[["\""]]) == '\"')                        -- c0
assert(DecodeJson(EncodeJson"it's wonderful") == "it's wonderful")
assert(EncodeJson"it's wonderful" == '"it\\u0027s wonderful"')
assert(assert(DecodeJson[["\u0100"]]) == 'Ā')                     -- latin-1
assert(assert(DecodeJson[["\ud800\udf30\ud800\udf30"]]) == '𐌰𐌰')  -- utf-16 astral planes gothic
assert(assert(DecodeJson[["\uD800"]]) == '\\uD800')               -- utf-16 invalid (keep utf-8 well-formed)
assert(not DecodeJson('"\xc0\x80"'))
assert(not DecodeJson('"\xc1\x80"'))
assert(DecodeJson('"\xc2\x80"'))

assert(EncodeJson(assert(DecodeJson[[ -9223372036854775808  ]])) == '-9223372036854775808')  -- minimum 64-bit integer
assert(EncodeJson(assert(DecodeJson[[  9223372036854775807  ]])) ==  '9223372036854775807')  -- maximum 64-bit integer
assert(EncodeJson(assert(DecodeJson[[  9223372036854775808  ]])) ==  '9223372036854776000')  -- switches to double due to integer overflow
assert(EncodeJson(assert(DecodeJson[[ -9223372036854775809  ]])) == '-9223372036854776000')  -- switches to double due to integer underflow
assert(EncodeJson(assert(DecodeJson[[ 9223372036854775807.0 ]])) ==  '9223372036854776000')  -- switches to double due to period mark
assert(EncodeJson(assert(DecodeJson[[ 2.7182818284590452354 ]])) == '2.718281828459045')     -- euler constant w/ 17 digit precision
assert( EncodeLua(assert(DecodeJson[[ 2.7182818284590452354 ]])) == '2.718281828459045')     -- euler constant w/ 17 digit precision

res, err = DecodeJson[[ null ]]
assert(res == nil)
assert(err == nil)

res, err = DecodeJson[[ false ]]
assert(res == false)
assert(err == nil)

res, err = DecodeJson[[     ]]
assert(not res)
assert(err == 'unexpected eof')

res, err = DecodeJson[[ {} {} ]]
assert(not res)
assert(err == "junk after expression")

res, err = DecodeJson[[ {3:4} ]]
assert(not res)
assert(err == "object key must be string")

res, err = DecodeJson[[ z ]]
assert(not res)
assert(err == "illegal character")

res, err = DecodeJson[[ "\e" ]]
assert(not res)
assert(err == "invalid escape character")

res, err = DecodeJson[[ {"key": } ]]
assert(not res)
assert(err == "unexpected '}'")

res, err = DecodeJson[[ {"key": ] ]]
assert(not res)
assert(err == "unexpected ']'")

res, err = DecodeJson[[ {"key": ]]
assert(not res)
assert(err == "unexpected eof")

res, err = DecodeJson[[ {true:3} ]]
assert(not res)
assert(err == "object key must be string")

res, err = DecodeJson('"\x00"')
assert(res == nil)
assert(err == 'non-del c0 control code in string')

res, err = DecodeJson('"e')
assert(res == nil)
assert(err == 'unexpected eof in string')

res, err = DecodeJson('"\\xcc\\xa9"')
assert(res == nil)
assert(err == 'hex escape not printable')

res, err = DecodeJson('"\\xcj"')
assert(res == nil)
assert(err == 'invalid hex escape')

res, err = DecodeJson('"\\ucjcc"')
assert(res == nil)
assert(err == 'invalid unicode escape')

-- 63 objects
res, err = DecodeJson([[
{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":
{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":
{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":
{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":
{"k":{"k":{"k":{"k":0}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
}}}}}}}}}
]])
assert(res)

-- 64 objects
res, err = DecodeJson([[
{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":
{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":
{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":
{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":{"k":
{"k":{"k":{"k":{"k":0}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
}}}}}}}}}}
]])
assert(res == nil)
assert(err == "maximum depth exceeded")

--------------------------------------------------------------------------------
-- benchmark		nanos	ticks
--------------------------------------------------------------------------------
-- JsonParseEmpty	41	127
-- JsonParseInteger	66	206
-- JsonParseDouble	123	383
-- JsonParseString	116	361
-- JsonParseInts	256	793
-- JsonParseFloats	361	1118
-- JsonParseObject	890	2756
-- JsonEncodeInts	498	1543
-- JsonEncodeFloats	498	1543
-- JsonEncodeObject	1333	4129
-- BigString		3183	9855

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

function BigString()
   assert(DecodeJson[[
     ["The fall of Hyperion - a DreamJohn KeatsCANTO I𝘍𝘢𝘯𝘢𝘵𝘪𝘤𝘴 𝘩𝘢𝘷𝘦 𝘵𝘩𝘦𝘪𝘳 dreams, 𝘄𝗵𝗲𝗿𝗲𝘄𝗶𝘁𝗵 𝘁𝗵𝗲𝘆 𝘄𝗲𝗮𝘃𝗲A paradise for a sect; the savage tooFrom forth the loftiest fashion of his sleepGuesses at Heaven; pity these have notTrac'd upon vellum or wild Indian leafThe shadows of melodious utterance.But bare of laurel they live, dream, and die;For Poesy alone can tell her dreams,With the fine spell of words alone can saveImagination from the sable charmAnd dumb enchantment. Who alive can say,'Thou art no Poet may'st not tell thy dreams?'Since every man whose soul is not a clodHath visions, and would speak, if he had lovedAnd been well nurtured in his mother tongue.Whether the dream now purpos'd to rehearseBe poet's or fanatic's will be knownWhen this warm scribe my hand is in the grave.",
      "The fall of Hyperion - a DreamJohn KeatsCANTO I𝘍𝘢𝘯𝘢𝘵𝘪𝘤𝘴 𝘩𝘢𝘷𝘦 𝘵𝘩𝘦𝘪𝘳 dreams, 𝘄𝗵𝗲𝗿𝗲𝘄𝗶𝘁𝗵 𝘁𝗵𝗲𝘆 𝘄𝗲𝗮𝘃𝗲A paradise for a sect; the savage tooFrom forth the loftiest fashion of his sleepGuesses at Heaven; pity these have notTrac'd upon vellum or wild Indian leafThe shadows of melodious utterance.But bare of laurel they live, dream, and die;For Poesy alone can tell her dreams,With the fine spell of words alone can saveImagination from the sable charmAnd dumb enchantment. Who alive can say,'Thou art no Poet may'st not tell thy dreams?'Since every man whose soul is not a clodHath visions, and would speak, if he had lovedAnd been well nurtured in his mother tongue.Whether the dream now purpos'd to rehearseBe poet's or fanatic's will be knownWhen this warm scribe my hand is in the grave."]
   ]])
end

function JsonParseInts()
   DecodeJson[[ [123,456,789] ]]
end

function JsonParseFloats()
   DecodeJson[[ [3.14,1.23,7.89] ]]
end

function JsonParseObject()
   DecodeJson[[ {"3": "1", "4": "1", "5": {"3":"1", "4":"1", "5":"9"}} ]]
end

function JsonEncodeInts()
   EncodeJson({2, 0, {5, 7, 3}})
end

function JsonEncodeFloats()
   EncodeJson({3.14,1.23,7.89})
end

function JsonEncodeObject()
   EncodeJson({["3"]="1", ["4"]="1", ["5"]={["3"]="1", ["4"]="1", ["5"]="9"}})
end

function bench()
   print('JsonParseEmpty', Benchmark(JsonParseEmpty))
   print('JsonParseInteg', Benchmark(JsonParseInteger))
   print('JsonParseDouble', Benchmark(JsonParseDouble))
   print('JsonParseString', Benchmark(JsonParseString))
   print('JsonParseInts', Benchmark(JsonParseInts))
   print('JsonParseFloats', Benchmark(JsonParseFloats))
   print('JsonParseObject', Benchmark(JsonParseObject))
   print('JsonEncodeInts', Benchmark(JsonEncodeInts))
   print('JsonEncodeFlts', Benchmark(JsonEncodeFloats))
   print('JsonEncodeObj', Benchmark(JsonEncodeObject))
   print('BigString', Benchmark(BigString))
end
