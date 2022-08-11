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

assert(unix.pledge("stdio"))

-- these test cases are prefixed with n_
-- ljson should reject all of them as invalid

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_no-colon.json
val, err = DecodeJson(' {"a" ')
assert(val == nil)
assert(err == "unexpected eof")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_missing_value.json
val, err = DecodeJson(' {"a": ')
assert(val == nil)
assert(err == "unexpected eof")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_missing_key.json
val, err = DecodeJson(' {:"b"} ')
assert(val == nil)
assert(err == "unexpected ':'")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_missing_colon.json
val, err = DecodeJson(' {"a" b} ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_key_with_single_quotes.json
val, err = DecodeJson(' {key: \'value\'} ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_garbage_at_end.json
val, err = DecodeJson(' {"a":"a" 123} ')
assert(val == nil)
assert(err == "object key must be string")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_emoji.json
 -- (converted to binary for safety)
val, err = DecodeJson(' \x7b\xf0\x9f\x87\xa8\xf0\x9f\x87\xad\x7d ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_bracket_key.json
val, err = DecodeJson(' {[: "x"} ')
assert(val == nil)
assert(err == "object key must be string")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_with_alpha_char.json
val, err = DecodeJson(' [1.8011670033376514H-308] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_with_alpha.json
val, err = DecodeJson(' [1.2a-3] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_starting_with_dot.json
val, err = DecodeJson(' [.123] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_real_with_invalid_utf8_after_e.json
val, err = DecodeJson(" [1e\xe5] ")
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_real_garbage_after_e.json
val, err = DecodeJson(' [1ea] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_neg_with_garbage_at_end.json
val, err = DecodeJson(' [-1x] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_neg_real_without_int_part.json
val, err = DecodeJson(' [-.123] ')
assert(val == nil)
assert(err == "bad negative")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_minus_sign_with_trailing_garbage.json
val, err = DecodeJson(' [-foo] ')
assert(val == nil)
assert(err == "bad negative")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_minus_infinity.json
val, err = DecodeJson(' [-Infinity] ')
assert(val == nil)
assert(err == "bad negative")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid-utf-8-in-int.json
 -- (converted to binary for safety)
val, err = DecodeJson(' \x5b\x30\xe5\x5d ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid-utf-8-in-exponent.json
 -- (converted to binary for safety)
val, err = DecodeJson(' \x5b\x31\x65\x31\xe5\x5d ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid-utf-8-in-bigger-int.json
 -- (converted to binary for safety)
val, err = DecodeJson(' \x5b\x31\x32\x33\xe5\x5d ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid-negative-real.json
 -- (converted to binary for safety)
val, err = DecodeJson(' \x5b\x2d\x31\x32\x33\x2e\x31\x32\x33\x66\x6f\x6f\x5d ')
assert(val == nil)
assert(err == "missing ','")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_invalid+-.json
 -- (converted to binary for safety)
val, err = DecodeJson(" [0e+-1] ")
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_infinity.json
val, err = DecodeJson(' [Infinity] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_hex_2_digits.json
val, err = DecodeJson(' [0x42] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_hex_1_digit.json
val, err = DecodeJson(' [0x1] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_expression.json
val, err = DecodeJson(' [1+2] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_U+FF11_fullwidth_digit_one.json
 -- (converted to binary for safety)
val, err = DecodeJson(' \x5b\xef\xbc\x91\x5d ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_NaN.json
val, err = DecodeJson(' [NaN] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_Inf.json
val, err = DecodeJson(' [Inf] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_9.e+.json
val, err = DecodeJson(' [9.e+] ')
assert(val == nil)
assert(err == "bad double")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_1eE2.json
val, err = DecodeJson(' [1eE2] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_1.0e.json
val, err = DecodeJson(' [1.0e] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_1.0e-.json
val, err = DecodeJson(' [1.0e-] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_1.0e+.json
val, err = DecodeJson(' [1.0e+] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0e.json
val, err = DecodeJson(' [0e] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0e+.json
val, err = DecodeJson(' [0e+] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0_capital_E.json
val, err = DecodeJson(' [0E] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0_capital_E+.json
val, err = DecodeJson(' [0E+] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0.3e.json
val, err = DecodeJson(' [0.3e] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0.3e+.json
val, err = DecodeJson(' [0.3e+] ')
assert(val == nil)
assert(err == "bad exponent")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0.1.2.json
val, err = DecodeJson(' [0.1.2] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_.2e-3.json
val, err = DecodeJson(' [.2e-3] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_.-1.json
val, err = DecodeJson(' [.-1] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_-NaN.json
val, err = DecodeJson(' [-NaN] ')
assert(val == nil)
assert(err == "bad negative")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_-1.0..json
val, err = DecodeJson(' [-1.0.] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_+Inf.json
val, err = DecodeJson(' [+Inf] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_+1.json
val, err = DecodeJson(' [+1] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_++.json
val, err = DecodeJson(' [++1234] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_incomplete_true.json
val, err = DecodeJson(' [tru] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_incomplete_null.json
val, err = DecodeJson(' [nul] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_incomplete_false.json
val, err = DecodeJson(' [fals] ')
assert(val == nil)
assert(err == "illegal character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_unclosed_with_object_inside.json
assert(not DecodeJson(' [{} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_unclosed_with_new_lines.json
assert(not DecodeJson([[
[1,
1
,1  ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_unclosed_trailing_comma.json
assert(not DecodeJson(' [1, '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_unclosed.json
assert(not DecodeJson(' ["" '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_star_inside.json
assert(not DecodeJson(' [*] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_spaces_vertical_tab_formfeed.json
 -- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x0b\x61\x22\x5c\x66\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_newlines_unclosed.json
assert(not DecodeJson([[
["a",
4
,1,  ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_items_separated_by_semicolon.json
assert(not DecodeJson(' [1:2] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_invalid_utf8.json
 -- (converted to binary for safety)
assert(not DecodeJson(' \x5b\xff\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_incomplete_invalid_value.json
 -- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x78 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_incomplete.json
assert(not DecodeJson(' ["x" '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_colon_instead_of_comma.json
assert(not DecodeJson(' ["": 1] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_a_invalid_utf8.json
assert(not DecodeJson(" [a\xe5] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_comma_instead_of_colon.json
assert(not DecodeJson(' {"x", null} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_bad_value.json
assert(not DecodeJson(' ["x", truth] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_multidigit_number_then_00.json
assert(not DecodeJson(' 123\x00 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_extra_close.json
-- (added spaces between [[ and ]] so lua doesn't get confused)
assert(not DecodeJson([[
[ "x"] ]   ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_with_leading_zero.json
assert(not DecodeJson(' [012] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_neg_int_starting_with_zero.json
assert(not DecodeJson(' [-012] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_1_000.json
assert(not DecodeJson(' [1 000.0] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_-01.json
assert(not DecodeJson(' [-01] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_minus_space_1.json
assert(not DecodeJson(' [- 1] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_just_minus.json
assert(not DecodeJson(' [-] '))

-- [jart] v8 permits the \xb9 but doesn't permit the trailing comma
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_lone_continuation_byte_in_key_and_trailing_comma.json
assert(not DecodeJson(" {\"\xb9\":\"0\",} "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_double_colon.json
assert(not DecodeJson(' {"x"::"b"} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_number_and_several_commas.json
assert(not DecodeJson(' [1,,] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_number_and_comma.json
assert(not DecodeJson(' [1,] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_missing_value.json
assert(not DecodeJson(' [   , ""] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_just_comma.json
assert(not DecodeJson(' [,] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_extra_comma.json
assert(not DecodeJson(' ["",] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_double_extra_comma.json
assert(not DecodeJson(' ["x",,] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_double_comma.json
assert(not DecodeJson(' [1,,2] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_comma_and_number.json
assert(not DecodeJson(' [,1] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_inner_array_no_comma.json
-- (added spaces between [[ and ]] so lua doesn't get confused)
assert(not DecodeJson([[
[ 3[ 4] ]   ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_1_true_without_comma.json
assert(not DecodeJson(' [1 true] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_missing_semicolon.json
assert(not DecodeJson(' {"a" "b"} '))

-- lool
assert(not DecodeJson(' [--2.] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_real_without_fractional_part.json
assert(not DecodeJson(' [1.] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_2.e3.json
assert(not DecodeJson(' [2.e3] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_2.e-3.json
assert(not DecodeJson(' [2.e-3] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_2.e+3.json
assert(not DecodeJson(' [2.e+3] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_0.e1.json
assert(not DecodeJson(' [0.e1] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_number_-2..json
assert(not DecodeJson(' [-2.] '))
