
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


-- these test cases are prefixed with y_
-- ljson should accept all of them as valid

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_whitespace_array.json
assert(true == pcall(DecodeJson, [[  [] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_true_in_array.json
assert(true == pcall(DecodeJson, [[ [true] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_trailing_newline.json
assert(true == pcall(DecodeJson, [[ ["a"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_string_empty.json
assert(true == pcall(DecodeJson, [[ "" ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_lonely_true.json
assert(true == pcall(DecodeJson, [[ true ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_lonely_string.json
assert(true == pcall(DecodeJson, [[ "asd" ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_lonely_null.json
assert(true == pcall(DecodeJson, [[ null ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_lonely_negative_real.json
assert(true == pcall(DecodeJson, [[ -0.1 ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_lonely_int.json
assert(true == pcall(DecodeJson, [[ 42 ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_structure_lonely_false.json
assert(true == pcall(DecodeJson, [[ false ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_with_del_character.json
assert(true == pcall(DecodeJson, [[ ["aa"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_utf8.json
assert(true == pcall(DecodeJson, [[ ["€𝄞"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicode_escaped_double_quote.json
assert(true == pcall(DecodeJson, [[ ["\u0022"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicode_U+FFFE_nonchar.json
assert(true == pcall(DecodeJson, [[ ["\uFFFE"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicode_U+FDD0_nonchar.json
assert(true == pcall(DecodeJson, [[ ["\uFDD0"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicode_U+2064_invisible_plus.json
assert(true == pcall(DecodeJson, [[ ["\u2064"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json
assert(true == pcall(DecodeJson, [[ ["\u200B"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicode_U+1FFFE_nonchar.json
assert(true == pcall(DecodeJson, [[ ["\uD83F\uDFFE"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicode_U+10FFFE_nonchar.json
assert(true == pcall(DecodeJson, [[ ["\uDBFF\uDFFE"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicode_2.json
assert(true == pcall(DecodeJson, [[ ["⍂㈴⍂"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicodeEscapedBackslash.json
assert(true == pcall(DecodeJson, [[ ["\u005C"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unicode.json
assert(true == pcall(DecodeJson, [[ ["\uA66D"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_unescaped_char_delete.json
assert(true == pcall(DecodeJson, [[ [""] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_uescaped_newline.json
assert(true == pcall(DecodeJson, [[ ["new\u000Aline"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_uEscape.json
assert(true == pcall(DecodeJson, [[ ["\u0061\u30af\u30EA\u30b9"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_u+2029_par_sep.json
assert(true == pcall(DecodeJson, [[ [" "] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_u+2028_line_sep.json
assert(true == pcall(DecodeJson, [[ [" "] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_two-byte-utf-8.json
assert(true == pcall(DecodeJson, [[ ["\u0123"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_three-byte-utf-8.json
assert(true == pcall(DecodeJson, [[ ["\u0821"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json
assert(true == pcall(DecodeJson, [[ ["\uD834\uDd1e"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_space.json
assert(true == pcall(DecodeJson, [[ " " ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_simple_ascii.json
assert(true == pcall(DecodeJson, [[ ["asd "] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_reservedCharacterInUTF-8_U+1BFFF.json
assert(true == pcall(DecodeJson, [[ ["𛿿"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_pi.json
assert(true == pcall(DecodeJson, [[ ["π"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_one-byte-utf-8.json
assert(true == pcall(DecodeJson, [[ ["\u002c"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_null_escape.json
assert(true == pcall(DecodeJson, [[ ["\u0000"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_nonCharacterInUTF-8_U+FFFF.json
assert(true == pcall(DecodeJson, [[ ["￿"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_nonCharacterInUTF-8_U+10FFFF.json
assert(true == pcall(DecodeJson, [[ ["􏿿"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_nbsp_uescaped.json
assert(true == pcall(DecodeJson, [[ ["new\u00A0line"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_last_surrogates_1_and_2.json
assert(true == pcall(DecodeJson, [[ ["\uDBFF\uDFFF"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_in_array_with_leading_space.json
assert(true == pcall(DecodeJson, [[ [ "asd"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_in_array.json
assert(true == pcall(DecodeJson, [[ ["asd"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_escaped_noncharacter.json
assert(true == pcall(DecodeJson, [[ ["\uFFFF"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_escaped_control_character.json
assert(true == pcall(DecodeJson, [[ ["\u0012"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_double_escape_n.json
assert(true == pcall(DecodeJson, [[ ["\\n"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_double_escape_a.json
assert(true == pcall(DecodeJson, [[ ["\\a"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_comments.json
assert(true == pcall(DecodeJson, [[ ["a/*b*/c/*d//e"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_backslash_doublequotes.json
assert(true == pcall(DecodeJson, [[ ["\""] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_backslash_and_u_escaped_zero.json
assert(true == pcall(DecodeJson, [[ ["\\u0000"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_allowed_escapes.json
assert(true == pcall(DecodeJson, [[ ["\"\\\/\b\f\n\r\t"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_accepted_surrogate_pairs.json
assert(true == pcall(DecodeJson, [[ ["\ud83d\ude39\ud83d\udc8d"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_accepted_surrogate_pair.json
assert(true == pcall(DecodeJson, [[ ["\uD801\udc37"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_string_1_2_3_bytes_UTF-8_sequences.json
assert(true == pcall(DecodeJson, [[ ["\u0060\u012a\u12AB"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_with_newlines.json
assert(true == pcall(DecodeJson, [[
{
"a": "b"
}  ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_string_unicode.json
assert(true == pcall(DecodeJson, [[ {"title":"\u041f\u043e\u043b\u0442\u043e\u0440\u0430 \u0417\u0435\u043c\u043b\u0435\u043a\u043e\u043f\u0430" } ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_simple.json
assert(true == pcall(DecodeJson, [[ {"a":[]} ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_long_strings.json
assert(true == pcall(DecodeJson, [[ {"x":[{"id": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"}], "id": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"} ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_extreme_numbers.json
assert(true == pcall(DecodeJson, [[ { "min": -1.0e+28, "max": 1.0e+28 } ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_escaped_null_in_key.json
assert(true == pcall(DecodeJson, [[ {"foo\u0000bar": 42} ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_empty_key.json
assert(true == pcall(DecodeJson, [[ {"":0} ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_empty.json
assert(true == pcall(DecodeJson, [[ {} ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_duplicated_key_and_value.json
assert(true == pcall(DecodeJson, [[ {"a":"b","a":"b"} ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_duplicated_key.json
assert(true == pcall(DecodeJson, [[ {"a":"b","a":"c"} ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object_basic.json
assert(true == pcall(DecodeJson, [[ {"asd":"sdf"} ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_object.json
assert(true == pcall(DecodeJson, [[ {"asd":"sdf", "dfg":"fgh"} ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_simple_real.json
assert(true == pcall(DecodeJson, [[ [123.456789] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_simple_int.json
assert(true == pcall(DecodeJson, [[ [123] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_real_pos_exponent.json
assert(true == pcall(DecodeJson, [[ [1e+2] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_real_neg_exp.json
assert(true == pcall(DecodeJson, [[ [1e-2] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_real_fraction_exponent.json
assert(true == pcall(DecodeJson, [[ [123.456e78] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_real_exponent.json
assert(true == pcall(DecodeJson, [[ [123e45] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_real_capital_e_pos_exp.json
assert(true == pcall(DecodeJson, [[ [1E+2] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_real_capital_e_neg_exp.json
assert(true == pcall(DecodeJson, [[ [1E-2] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_real_capital_e.json
assert(true == pcall(DecodeJson, [[ [1E22] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_negative_zero.json
assert(true == pcall(DecodeJson, [[ [-0] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_negative_one.json
assert(true == pcall(DecodeJson, [[ [-1] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_negative_int.json
assert(true == pcall(DecodeJson, [[ [-123] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_minus_zero.json
assert(true == pcall(DecodeJson, [[ [-0] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_int_with_exp.json
assert(true == pcall(DecodeJson, [[ [20e1] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_double_close_to_zero.json
assert(true == pcall(DecodeJson, [[ [-0.000000000000000000000000000000000000000000000000000000000000000000000000000001] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_after_space.json
assert(true == pcall(DecodeJson, [[ [ 4] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_0e1.json
assert(true == pcall(DecodeJson, [[ [0e1] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number_0e+1.json
assert(true == pcall(DecodeJson, [[ [0e+1] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_number.json
assert(true == pcall(DecodeJson, [[ [123e65] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_with_trailing_space.json
assert(true == pcall(DecodeJson, [[ [2] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_with_several_null.json
assert(true == pcall(DecodeJson, [[ [1,null,null,null,2] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_with_leading_space.json
assert(true == pcall(DecodeJson, [[  [1] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_with_1_and_newline.json
assert(true == pcall(DecodeJson, [[
[1
]  ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_null.json
assert(true == pcall(DecodeJson, [[ [null] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_heterogeneous.json
assert(true == pcall(DecodeJson, [[ [null, 1, "1", {}] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_false.json
assert(true == pcall(DecodeJson, [[ [false] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_ending_with_newline.json
assert(true == pcall(DecodeJson, [[ ["a"] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_empty.json
assert(true == pcall(DecodeJson, [[ [] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_empty-string.json
assert(true == pcall(DecodeJson, [[ [""] ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/y_array_arraysWithSpaces.json
-- (added spaces between [[ and ]] so lua doesn't get confused)
assert(true == pcall(DecodeJson, [[
[ [ ]    ]   ]]))

