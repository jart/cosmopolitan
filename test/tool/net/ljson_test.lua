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

assert(EncodeLua(ParseJson[[     ]]) == 'nil')
assert(EncodeLua(ParseJson[[  0  ]]) ==  '0' )
assert(EncodeLua(ParseJson[[ [1] ]]) == '{1}')
assert(EncodeLua(ParseJson[[ 2.3 ]]) == '2.3')
assert(EncodeLua(ParseJson[[ [1,3,2] ]]) == '{1, 3, 2}')
assert(EncodeLua(ParseJson[[ {1: 2, 3: 4} ]]) == '{[1]=2, [3]=4}')
assert(EncodeLua(ParseJson[[ {"foo": 2, "bar": 4} ]]) == '{bar=4, foo=2}')
assert(EncodeLua(ParseJson[[ -123 ]]) == '-123')
assert(EncodeLua(ParseJson[[ 1.e6 ]]) == '1000000')
assert(EncodeLua(ParseJson[[ 1.e-6 ]]) == '1e-06')
assert(EncodeLua(ParseJson[[ 1e-06 ]]) == '1e-06')
assert(EncodeLua(ParseJson[[ 9.123e6 ]]) == '9123000')
assert(EncodeLua(ParseJson[[ [{"heh": [1,3,2]}] ]]) == '{{heh={1, 3, 2}}}')
assert(EncodeLua(ParseJson[[ 3.14159 ]]) == '3.14159')
assert(EncodeLua(ParseJson[[ {3=4} ]]) == '{[3]=4}')
assert(EncodeLua(ParseJson[[ 1e-12 ]]) == '1e-12')
assert(EncodeJson(ParseJson[[ 1e-12 ]]) == '1e-12')

----------------------------------------------------------------------------------------------------
-- benchmarks

function JsonParseEmpty()
   ParseJson[[]]
end

function JsonParseInt()
   ParseJson[[ 314159 ]]
end

function JsonParseDouble()
   ParseJson[[ 3.14159 ]]
end

function JsonParseArray()
   ParseJson[[ [3,1,4,1,5,9] ]]
end

function JsonParseObject()
   ParseJson[[ {"3":"1","4":"1","5":"9"} ]]
end

print('JsonParseEmpty', Benchmark(JsonParseEmpty))
print('JsonParseInt', Benchmark(JsonParseInt))
print('JsonParseDouble', Benchmark(JsonParseDouble))
print('JsonParseArray', Benchmark(JsonParseArray))
print('JsonParseObject', Benchmark(JsonParseObject))

--			nanos	ticks
-- JsonParseEmpty	24	77	85	1
-- JsonParseInt		31	96	82	1
-- JsonParseDouble	64	199	82	1
-- JsonParseArray	367	1139	80	1
-- JsonParseObject	425	1317	79	1
