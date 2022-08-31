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

assert(EncodeJson(nil) == "null")
assert(EncodeJson(true) == "true")
assert(EncodeJson(false) == "false")
assert(EncodeJson(0) == "0")
assert(EncodeJson(0.0) == "0")
assert(EncodeJson(3.14) == "3.14")
assert(EncodeJson(0/0) == "null")
assert(EncodeJson(math.huge) == "null")
assert(EncodeJson(123.456e-789) == '0')
assert(EncodeJson(9223372036854775807) == '9223372036854775807')
assert(EncodeJson(-9223372036854775807 - 1) == '-9223372036854775808')
assert(EncodeJson({2, 1}) == "[2,1]")
assert(EncodeJson({3, 2}) == "[3,2]")
assert(EncodeJson({[0]=false}) == "[]")
assert(EncodeJson({[1]=123, [2]=456}) == "[123,456]")
assert(EncodeJson({[1]=123, [3]=456}) == "[123]")
assert(EncodeJson({["hi"]=1, [1]=2}) == "[2]")
assert(EncodeJson({[1]=2, ["hi"]=1}) == "[2]")
assert(EncodeJson({[3]=3, [1]=3}) == "[3]")
assert(EncodeJson("hello") == "\"hello\"")
assert(EncodeJson("\x00") == "\"\\u0000\"")
assert(EncodeJson("\t") == "\"\\t\"")
assert(EncodeJson("→") == "\"\\u2192\"")
assert(EncodeJson("𐌰") == "\"\\ud800\\udf30\"")
assert(EncodeJson("\t") == [["\t"]])
assert(EncodeJson("\r") == [["\r"]])
assert(EncodeJson("\n") == [["\n"]])
assert(EncodeJson("\f") == [["\f"]])
assert(EncodeJson("\"") == [["\""]])
assert(EncodeJson("\'") == [["\u0027"]])
assert(EncodeJson("\\") == [["\\"]])

assert(EncodeJson(
          {yo=2,
           bye={yo=2,
                dawg=3},
           there={yo=2},
           sup={yo=2},
           hi="hello"},
          {pretty=true}) ==
         "{\n"..
         "  \"bye\": {\n"..
         "    \"dawg\": 3,\n"..
         "    \"yo\": 2\n"..
         "  },\n"..
         "  \"hi\": \"hello\",\n"..
         "  \"sup\": {\"yo\": 2},\n"..
         "  \"there\": {\"yo\": 2},\n"..
         "  \"yo\": 2\n"..
         "}")

assert(EncodeJson(
          {yo=2, bye=1, there=10, sup=3, hi="hello"},
          {pretty=true, indent="    "}) ==
       "{\n"..
       "    \"bye\": 1,\n"..
       "    \"hi\": \"hello\",\n"..
       "    \"sup\": 3,\n"..
       "    \"there\": 10,\n"..
       "    \"yo\": 2\n"..
       "}")

val, err = EncodeJson({[3]=3, [2]=3})
assert(val == nil)
assert(err == 'json objects must only use string keys')

val, err = EncodeJson({[{[{[3]=2}]=2}]=2})
assert(val == nil)
assert(err == 'json objects must only use string keys')

val, err = EncodeJson(EncodeJson)
assert(val == nil)
assert(err == "unsupported lua type")

x = {2, 1}
x[3] = x
val, err = EncodeJson(x)
assert(val == nil)
assert(err == "won't serialize cyclic lua table")

x = {}
x.c = 'c'
x.a = 'a'
x.b = 'b'
assert(EncodeJson(x) == '{"a":"a","b":"b","c":"c"}')

assert(EncodeJson(0, {maxdepth=1}))
val, err = EncodeJson(0, {maxdepth=0})
assert(val == nil)
assert(err == 'table has great depth')

assert(EncodeJson({{{{{{{{{{{},{{{{},{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}, {maxdepth=64}) ==
       '[[[[[[[[[[{},[[[{},[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[{}]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]')
val, err = EncodeJson({{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}})
assert(val == nil)
assert(err == 'table has great depth')

-- 63 objects
assert(EncodeJson(
{k={k={k={k={k={k={k={k={k={k={k={k={k={k=
{k={k={k={k={k={k={k={k={k={k={k={k={k={k={k=
{k={k={k={k={k={k={k={k={k={k={k={k={k={k={k=
{k={k={k={k={k={k={k={k={k={k={k={k={k={k={k=
{k={k={k={k=0}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}, {maxdepth=64}) ==
"{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":"..
"{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":"..
"{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":"..
"{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":{\"k\":0}}"..
"}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}"..
"}}}}}}}}}}}}}")

-- 64 objects
res, err = EncodeJson(
{k={k={k={k={k={k={k={k={k={k={k={k={k={k={k=
{k={k={k={k={k={k={k={k={k={k={k={k={k={k={k=
{k={k={k={k={k={k={k={k={k={k={k={k={k={k={k=
{k={k={k={k={k={k={k={k={k={k={k={k={k={k={k=
{k={k={k={k=0}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}, {maxdepth=64})
assert(res == nil)
assert(err == "table has great depth")

--------------------------------------------------------------------------------
-- benchmark		nanos	ticks
-- JsonEncArray		366	1134
-- JsonEncUnsort	754	2336
-- JsonEncObject	1208	3742

function JsonEncArray()
   EncodeJson({2, 1, 10, 3, "hello"})
end

function JsonEncObject()
   EncodeJson({yo=2, bye=1, there=10, sup=3, hi="hello"})
end

UNSORT = {sorted=false}
function JsonEncUnsort()
   EncodeJson({yo=2, bye=1, there=10, sup=3, hi="hello"}, UNSORT)
end

function bench()
   print("JsonEncArray", Benchmark(JsonEncArray))
   print("JsonEncUnsort", Benchmark(JsonEncUnsort))
   print("JsonEncObject", Benchmark(JsonEncObject))
end
