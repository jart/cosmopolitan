-- 
-- Nicolas Seriot's JSONTestSuite
-- https://github.com/nst/JSONTestSuite
-- commit d64aefb55228d9584d3e5b2433f720ea8fd00c82
-- 
-- MIT License
-- 
-- Copyright (c) 2016 Nicolas Seriot
-- 
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.
-- 

-- [jart] these tests deviate from the expectations of the upstream test
--        suite. most of these failures are because we permit syntax
--        like this since it saves bandwidth and makes the impl smaller.
--        we're also more permissive about things like the encoding of
--        double exponents and empty double fraction.
assert(EncodeLua(DecodeJson('[0 1 2 3 4]')) == '{0, 1, 2, 3, 4}')

-- from fail4.lua
--------------------------------------------------------------------------------

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_two_commas_in_a_row.json
assert(DecodeJson(' {"a":"b",,"c":"d"} '))
assert(EncodeLua(DecodeJson(' {"a":"b",,"c":"d"} ')) == '{a="b", c="d"}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_trailing_comma.json
assert(DecodeJson(' {"id":0,} '))
assert(EncodeLua(DecodeJson(' {"id":0,} ')) == '{id=0}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_several_trailing_commas.json
assert(DecodeJson(' {"id":0,,,,,} '))
assert(EncodeLua(DecodeJson(' {"id":0,,,,,} ')) == '{id=0}')

-- from fail1.lua
--------------------------------------------------------------------------------

-- [jart] v8 permits the \xb9 but doesn't permit the trailing comma
--        therefore this succeeds beacuse we don't care about comma
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_lone_continuation_byte_in_key_and_trailing_comma.json
assert(DecodeJson(" {\"\xb9\":\"0\",} "))
assert(EncodeLua(DecodeJson(" {\"\xb9\":\"0\",} ")) == '{["\\xb9"]="0"}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_missing_semicolon.json
assert(DecodeJson(' {"a" "b"} '))
assert(EncodeLua(DecodeJson(' {"a" "b"} ')) == '{a="b"}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_double_colon.json
assert(DecodeJson(' {"x"::"b"} '))
assert(EncodeLua(DecodeJson(' {"x"::"b"} ')) == '{x="b"}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_real_without_fractional_part.json
assert(DecodeJson(' [1.] '))
assert(EncodeLua(DecodeJson(' [1.] ')) == EncodeLua({1.0}))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_2.e3.json
assert(DecodeJson(' [2.e3] '))
assert(EncodeLua(DecodeJson(' [2.e3] ')) == '{2000.}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_2.e-3.json
assert(DecodeJson(' [2.e-3] '))
assert(EncodeLua(DecodeJson(' [2.e-3] ')) == '{0.002}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_2.e+3.json
assert(DecodeJson(' [2.e+3] '))
assert(EncodeLua(DecodeJson(' [2.e+3] ')) == '{2000.}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0.e1.json
assert(DecodeJson(' [0.e1] '))
assert(EncodeLua(DecodeJson(' [0.e1] ')) == '{0.}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_-2..json
assert(DecodeJson(' [-2.] '))
assert(EncodeLua(DecodeJson(' [-2.] ')) == '{-2.}')

-- lool
assert(not DecodeJson(' [--2.] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_number_and_several_commas.json
assert(DecodeJson(' [1,,] '))
assert(EncodeLua(DecodeJson(' [1,,] ')) == '{1}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_number_and_comma.json
assert(DecodeJson(' [1,] '))
assert(EncodeLua(DecodeJson(' [1,] ')) == '{1}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_missing_value.json
assert(DecodeJson(' [   , ""] '))
assert(EncodeLua(DecodeJson(' [   , ""] ')) == '{""}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_just_comma.json
assert(DecodeJson(' [,] '))
assert(EncodeLua(DecodeJson(' [,] ')) == EncodeLua(DecodeJson(' [] ')))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_inner_array_no_comma.json
-- (added spaces between [[ and ]] so lua doesn't get confused)
assert(DecodeJson([[
[ 3[ 4] ]   ]]))
assert(EncodeLua(DecodeJson([[
[ 3[ 4] ]   ]])) == '{3, {4}}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_extra_comma.json
assert(DecodeJson(' ["",] '))
assert(EncodeLua(DecodeJson(' ["",] ')) == '{""}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_double_extra_comma.json
assert(DecodeJson(' ["x",,] '))
assert(EncodeLua(DecodeJson(' ["x",,] ')) == '{"x"}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_double_comma.json
assert(DecodeJson(' [1,,2] '))
assert(EncodeLua(DecodeJson(' [1,,2] ')) == '{1, 2}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_comma_and_number.json
assert(DecodeJson(' [,1] '))
assert(EncodeLua(DecodeJson(' [,1] ')) == '{1}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_1_true_without_comma.json
assert(DecodeJson(' [1 true] '))
assert(EncodeLua(DecodeJson(' [1 true] ')) == '{1, true}')
