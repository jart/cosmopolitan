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

-- from fail1.lua
--------------------------------------------------------------------------------

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_missing_semicolon.json
assert(DecodeJson(' {"a" "b"} '))
assert(EncodeLua(DecodeJson(' {"a" "b"} ')) == '{a="b"}')

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

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_inner_array_no_comma.json
-- (added spaces between [[ and ]] so lua doesn't get confused)
assert(DecodeJson([[
[ 3[ 4] ]   ]]))
assert(EncodeLua(DecodeJson([[
[ 3[ 4] ]   ]])) == '{3, {4}}')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_1_true_without_comma.json
assert(DecodeJson(' [1 true] '))
assert(EncodeLua(DecodeJson(' [1 true] ')) == '{1, true}')
