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

-- these test cases are prefixed with i_
-- ljson is free to accept or reject,
-- but we run them anyway to check for segfaults

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_structure_UTF-8_BOM_empty_object.json
-- (converted to binary for safety)
assert(not DecodeJson(' \xef\xbb\xbf{} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_structure_500_nested_arrays.json
-- (added spaces between [[ and ]] so lua doesn't get confused)
assert(not DecodeJson([[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [
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
val, err = DecodeJson(" [\x00\"\x00\xe9\x00\"\x00]\x00 ")
assert(val == nil)
assert(err == 'illegal character')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_utf16BE_no_BOM.json
assert(not DecodeJson(" \x00[\x00\"\x00\xe9\x00\"\x00] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_truncated-utf-8.json
val, err = DecodeJson(" [\"\xe0\xff\"] ")
assert(val == nil)
assert(err == 'malformed utf-8')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_overlong_sequence_6_bytes_null.json
val, err = DecodeJson(" [\"\xfc\x80\x80\x80\x80\x80\"] ")
assert(val == nil)
assert(err == 'illegal utf-8 character')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_overlong_sequence_6_bytes.json
val, err = DecodeJson(" [\"\xfc\x83\xbf\xbf\xbf\xbf\"] ")
assert(val == nil)
assert(err == "illegal utf-8 character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_overlong_sequence_2_bytes.json
val, err = DecodeJson(" [\"\xc0\xaf\"] ")
assert(val == nil)
assert(err == "overlong ascii")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_not_in_unicode_range.json
val, err = DecodeJson(" [\"\xf4\xbf\xbf\xbf\"] ")
assert(val == nil)
assert(err == "utf-8 exceeds utf-16 range")

-- [jart] ljson currently doesn't validate utf-8
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_lone_utf8_continuation_byte.json
val, err = DecodeJson(" [\"\x81\"] ")
assert(val == nil)
assert(err == "c1 control code in string")

-- [jart] our behavior here is consistent with v8
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_lone_second_surrogate.json
assert(DecodeJson(" [\"\\uDFAA\"] "))
assert(EncodeJson(DecodeJson(" [\"\\uDFAA\"] ")) == "[\"\\\\uDFAA\"]")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_iso_latin_1.json
val, err = DecodeJson(" [\"\xe9\"] ")
assert(val == nil)
assert(err == "malformed utf-8")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_inverted_surrogates_U+1D11E.json
assert(DecodeJson(" [\"\\uDd1e\\uD834\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_invalid_utf-8.json
val, err = DecodeJson(" [\"\xff\"] ")
assert(val == nil)
assert(err == "illegal utf-8 character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_invalid_surrogate.json
assert(EncodeJson(DecodeJson(" [\"\\ud800abc\"] ")) == "[\"\\\\ud800abc\"]")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_invalid_lonely_surrogate.json
assert(DecodeJson(" [\"\\ud800\"] ")[1] == "\\ud800")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_incomplete_surrogates_escape_valid.json
assert(DecodeJson(" [\"\\uD800\\uD800\\n\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_incomplete_surrogate_pair.json
assert(DecodeJson(" [\"\\uDd1ea\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_incomplete_surrogate_and_escape_valid.json
assert(DecodeJson(" [\"\\uD800\\n\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_UTF8_surrogate_U+D800.json
assert(DecodeJson(" [\"\\ud800abc\"] ")[1] == "\\ud800abc")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_UTF-8_invalid_sequence.json
val, err = DecodeJson(" [\"\xe6\x97\xa5\xd1\x88\xfa\"] ")
assert(val == nil)
assert(err == "illegal utf-8 character")

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_UTF-16LE_with_BOM.json
assert(not DecodeJson(" \xff\xfe[\x00\"\x00\xe9\x00\"\x00]\x00 "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_1st_valid_surrogate_2nd_invalid.json
assert(DecodeJson(" [\"\\uD888\\u1234\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_string_1st_surrogate_but_2nd_missing.json
-- (converted to binary for safety)
assert(DecodeJson(" [\"\\uDADA\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_object_key_lone_2nd_surrogate.json
-- (converted to binary for safety)
assert(DecodeJson('  \x7b\x22\x5c\x75\x44\x46\x41\x41\x22\x3a\x30\x7d '))

-- [jart] ljson is precicely the same as v8 with integers larger than int64_t
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_very_big_negative_int.json
assert(DecodeJson(' [-237462374673276894279832749832423479823246327846] '))
assert(EncodeJson(DecodeJson(' [-237462374673276894279832749832423479823246327846] ')) == '[-2.374623746732769e+47]')

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_too_big_pos_int.json
assert(DecodeJson(' [100000000000000000000] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_too_big_neg_int.json
assert(DecodeJson(' [-123123123123123123123123123123] '))

-- [jart] once again consistent with v8
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_real_underflow.json
assert(DecodeJson(' [123e-10000000] '))
assert(EncodeJson(DecodeJson(' [123e-10000000] ')) == '[0]')
assert(EncodeLua(DecodeJson(' [123e-10000000] ')) == '{0.}')

-- [jart] consistent with v8 we encode Infinity as null (wut?)
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_real_pos_overflow.json
assert(DecodeJson(' [123123e100000] '))
assert(EncodeJson(DecodeJson(' [123123e100000] ')) == '[null]')

-- [jart] consistent with v8 we encode -Infinity as null (wut?)
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_real_neg_overflow.json
assert(DecodeJson(' [-123123e100000] '))
assert(EncodeJson(DecodeJson(' [-123123e100000] ')) == '[null]')

-- [jart] consistent with v8 we encode Infinity as null (wut?)
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_pos_double_huge_exp.json
assert(DecodeJson(' [1.5e+9999] '))
assert(EncodeJson(DecodeJson(' [1.5e+9999] ')) == '[null]')

-- [jart] consistent with v8 we encode -Infinity as null (wut?)
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_neg_int_huge_exp.json
assert(DecodeJson(' [-1e+9999] '))
assert(EncodeJson(DecodeJson(' [-1e+9999] ')) == '[null]')

-- [jart] consistent with v8 we encode Infinity as null (wut?)
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_huge_exp.json
assert(DecodeJson(' [0.4e00669999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999969999999006] '))
assert(EncodeJson(DecodeJson(' [0.4e00669999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999969999999006] ')) == '[null]')

-- [jart] consistent with v8 we encode underflow as 0
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/i_number_double_huge_neg_exp.json
assert(DecodeJson(' [123.456e-789] '))
assert(EncodeJson(DecodeJson(' [123.456e-789] ')) == '[0]')
