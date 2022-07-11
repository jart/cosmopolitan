
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

-- these test cases are prefixed with n_
-- ljson should reject all of them as invalid

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_no-colon.json
assert(false == pcall(DecodeJson, ' {"a" '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_missing_value.json
assert(false == pcall(DecodeJson, ' {"a": '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_missing_key.json
assert(false == pcall(DecodeJson, ' {:"b"} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_missing_colon.json
assert(false == pcall(DecodeJson, ' {"a" b} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_key_with_single_quotes.json
assert(false == pcall(DecodeJson, ' {key: \'value\'} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_garbage_at_end.json
assert(false == pcall(DecodeJson, ' {"a":"a" 123} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_emoji.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x7b\xf0\x9f\x87\xa8\xf0\x9f\x87\xad\x7d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_bracket_key.json
assert(false == pcall(DecodeJson, ' {[: "x"} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_with_alpha_char.json
assert(false == pcall(DecodeJson, ' [1.8011670033376514H-308] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_with_alpha.json
assert(false == pcall(DecodeJson, ' [1.2a-3] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_starting_with_dot.json
assert(false == pcall(DecodeJson, ' [.123] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_real_with_invalid_utf8_after_e.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x31\x65\xe5\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_real_garbage_after_e.json
assert(false == pcall(DecodeJson, ' [1ea] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_neg_with_garbage_at_end.json
assert(false == pcall(DecodeJson, ' [-1x] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_neg_real_without_int_part.json
assert(false == pcall(DecodeJson, ' [-.123] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_minus_sign_with_trailing_garbage.json
assert(false == pcall(DecodeJson, ' [-foo] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_minus_infinity.json
assert(false == pcall(DecodeJson, ' [-Infinity] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid-utf-8-in-int.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x30\xe5\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid-utf-8-in-exponent.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x31\x65\x31\xe5\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid-utf-8-in-bigger-int.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x31\x32\x33\xe5\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid-negative-real.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x2d\x31\x32\x33\x2e\x31\x32\x33\x66\x6f\x6f\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid+-.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x30\x65\x2b\x2d\x31\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_infinity.json
assert(false == pcall(DecodeJson, ' [Infinity] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_hex_2_digits.json
assert(false == pcall(DecodeJson, ' [0x42] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_hex_1_digit.json
assert(false == pcall(DecodeJson, ' [0x1] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_expression.json
assert(false == pcall(DecodeJson, ' [1+2] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_U+FF11_fullwidth_digit_one.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\xef\xbc\x91\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_NaN.json
assert(false == pcall(DecodeJson, ' [NaN] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_Inf.json
assert(false == pcall(DecodeJson, ' [Inf] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_9.e+.json
assert(false == pcall(DecodeJson, ' [9.e+] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_1eE2.json
assert(false == pcall(DecodeJson, ' [1eE2] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_1.0e.json
assert(false == pcall(DecodeJson, ' [1.0e] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_1.0e-.json
assert(false == pcall(DecodeJson, ' [1.0e-] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_1.0e+.json
assert(false == pcall(DecodeJson, ' [1.0e+] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0e.json
assert(false == pcall(DecodeJson, ' [0e] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0e+.json
assert(false == pcall(DecodeJson, ' [0e+] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0_capital_E.json
assert(false == pcall(DecodeJson, ' [0E] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0_capital_E+.json
assert(false == pcall(DecodeJson, ' [0E+] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0.3e.json
assert(false == pcall(DecodeJson, ' [0.3e] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0.3e+.json
assert(false == pcall(DecodeJson, ' [0.3e+] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0.1.2.json
assert(false == pcall(DecodeJson, ' [0.1.2] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_.2e-3.json
assert(false == pcall(DecodeJson, ' [.2e-3] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_.-1.json
assert(false == pcall(DecodeJson, ' [.-1] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_-NaN.json
assert(false == pcall(DecodeJson, ' [-NaN] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_-1.0..json
assert(false == pcall(DecodeJson, ' [-1.0.] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_+Inf.json
assert(false == pcall(DecodeJson, ' [+Inf] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_+1.json
assert(false == pcall(DecodeJson, ' [+1] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_++.json
assert(false == pcall(DecodeJson, ' [++1234] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_incomplete_true.json
assert(false == pcall(DecodeJson, ' [tru] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_incomplete_null.json
assert(false == pcall(DecodeJson, ' [nul] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_incomplete_false.json
assert(false == pcall(DecodeJson, ' [fals] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_unclosed_with_object_inside.json
assert(false == pcall(DecodeJson, ' [{} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_unclosed_with_new_lines.json
assert(false == pcall(DecodeJson, [[
[1,
1
,1  ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_unclosed_trailing_comma.json
assert(false == pcall(DecodeJson, ' [1, '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_unclosed.json
assert(false == pcall(DecodeJson, ' ["" '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_star_inside.json
assert(false == pcall(DecodeJson, ' [*] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_spaces_vertical_tab_formfeed.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x22\x0b\x61\x22\x5c\x66\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_newlines_unclosed.json
assert(false == pcall(DecodeJson, [[
["a",
4
,1,  ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_items_separated_by_semicolon.json
assert(false == pcall(DecodeJson, ' [1:2] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_invalid_utf8.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\xff\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_incomplete_invalid_value.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x78 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_incomplete.json
assert(false == pcall(DecodeJson, ' ["x" '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_colon_instead_of_comma.json
assert(false == pcall(DecodeJson, ' ["": 1] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_a_invalid_utf8.json
 -- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x61\xe5\x5d '))
