
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


-- these test cases are prefixed with i_
-- ljson is free to accept or reject,
-- but we run them anyway to check for segfaults

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_structure_UTF-8_BOM_empty_object.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \xef\xbb\xbf{} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_structure_500_nested_arrays.json
-- (added spaces between [[ and ]] so lua doesn't get confused)
assert(nil ~= pcall(DecodeJson, [[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]   ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_utf16LE_no_BOM.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x00\x22\x00\xe9\x00\x22\x00\x5d\x00 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_utf16BE_no_BOM.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x00\x5b\x00\x22\x00\xe9\x00\x22\x00\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_truncated-utf-8.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\xe0\xff\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_overlong_sequence_6_bytes_null.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\xfc\x80\x80\x80\x80\x80\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_overlong_sequence_6_bytes.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\xfc\x83\xbf\xbf\xbf\xbf\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_overlong_sequence_2_bytes.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\xc0\xaf\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_not_in_unicode_range.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\xf4\xbf\xbf\xbf\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_lone_utf8_continuation_byte.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x81\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_lone_second_surrogate.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x5c\x75\x44\x46\x41\x41\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_iso_latin_1.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\xe9\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_inverted_surrogates_U+1D11E.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x5c\x75\x44\x64\x31\x65\x5c\x75\x44\x38\x33\x34\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_invalid_utf-8.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\xff\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_invalid_surrogate.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x5c\x75\x64\x38\x30\x30\x61\x62\x63\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_invalid_lonely_surrogate.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x5c\x75\x64\x38\x30\x30\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_incomplete_surrogates_escape_valid.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x5c\x75\x44\x38\x30\x30\x5c\x75\x44\x38\x30\x30\x5c\x6e\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_incomplete_surrogate_pair.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x5c\x75\x44\x64\x31\x65\x61\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_incomplete_surrogate_and_escape_valid.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x5c\x75\x44\x38\x30\x30\x5c\x6e\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_UTF8_surrogate_U+D800.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\xed\xa0\x80\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_UTF-8_invalid_sequence.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\xe6\x97\xa5\xd1\x88\xfa\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_UTF-16LE_with_BOM.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \xff\xfe\x5b\x00\x22\x00\xe9\x00\x22\x00\x5d\x00 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_1st_valid_surrogate_2nd_invalid.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x5c\x75\x44\x38\x38\x38\x5c\x75\x31\x32\x33\x34\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_1st_surrogate_but_2nd_missing.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, ' \x5b\x22\x5c\x75\x44\x41\x44\x41\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_object_key_lone_2nd_surrogate.json
-- (converted to binary for safety)
assert(nil ~= pcall(DecodeJson, '  \x7b\x22\x5c\x75\x44\x46\x41\x41\x22\x3a\x30\x7d '))


-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_very_big_negative_int.json
assert(nil ~= pcall(DecodeJson, ' [-237462374673276894279832749832423479823246327846] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_too_big_pos_int.json
assert(nil ~= pcall(DecodeJson, ' [100000000000000000000] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_too_big_neg_int.json
assert(nil ~= pcall(DecodeJson, ' [-123123123123123123123123123123] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_real_underflow.json
assert(nil ~= pcall(DecodeJson, ' [123e-10000000] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_real_pos_overflow.json
assert(nil ~= pcall(DecodeJson, ' [123123e100000] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_real_neg_overflow.json
assert(nil ~= pcall(DecodeJson, ' [-123123e100000] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_pos_double_huge_exp.json
assert(nil ~= pcall(DecodeJson, ' [1.5e+9999] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_neg_int_huge_exp.json
assert(nil ~= pcall(DecodeJson, ' [-1e+9999] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_huge_exp.json
assert(nil ~= pcall(DecodeJson, ' [0.4e00669999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999969999999006] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_double_huge_neg_exp.json
assert(nil ~= pcall(DecodeJson, ' [123.456e-789] '))
