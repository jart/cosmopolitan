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

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_whitespace_formfeed.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x0c\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_whitespace_U+2060_word_joiner.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\xe2\x81\xa0\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unicode-identifier.json
-- (converted to binary for safety)
assert(not DecodeJson(' \xc3\xa5 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_object.json
assert(not DecodeJson(' {"asd":"asd" '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_array_unfinished_true.json
assert(not DecodeJson(' [ false, tru '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_array_unfinished_false.json
assert(not DecodeJson(' [ true, fals '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_array_partial_null.json
assert(not DecodeJson(' [ false, nul '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_array.json
assert(not DecodeJson(' [1 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_uescaped_LF_before_string.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x5c\x75\x30\x30\x30\x41\x22\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_single_star.json
assert(not DecodeJson(' * '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_single_eacute.json
assert(not DecodeJson(' é '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_open.json
assert(not DecodeJson([[ ["{["{["{["{ ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_string_with_apostrophes.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x7b\x27\x61\x27 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_open_string.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x7b\x22\x61 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_open_array.json
assert(not DecodeJson(' {[ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_comma.json
assert(not DecodeJson(' {, '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_close_array.json
assert(not DecodeJson(' {] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object.json
assert(not DecodeJson(' { '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_string.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x61\x22 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_open_string.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x61 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_open_object.json
assert(not DecodeJson(' [{ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_comma.json
assert(not DecodeJson(' [, '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_apostrophe.json
assert(not DecodeJson(' [\' '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_object_with_comment.json
assert(not DecodeJson(' {"a":/*comment*/"b"} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_object_unclosed_no_value.json
assert(not DecodeJson(' {"": '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_null-byte-outside-string.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x00\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_lone-open-bracket.json
assert(not DecodeJson(' [ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_lone-invalid-utf-8.json
-- (converted to binary for safety)
assert(not DecodeJson(' \xe5 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_incomplete_UTF8_BOM.json
assert(not DecodeJson(' ï»{} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_comma_instead_of_closing_brace.json
assert(not DecodeJson(' {"x": true, '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_capitalized_True.json
assert(not DecodeJson(' [True] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_ascii-unicode-identifier.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x61\xc3\xa5 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_array_with_unclosed_string.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x61\x73\x64\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_angle_bracket_null.json
assert(not DecodeJson(' [<null>] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_angle_bracket_..json
assert(not DecodeJson(' <.> '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_UTF8_BOM_no_data.json
assert(not DecodeJson(' ï»¿ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_U+2060_word_joined.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\xe2\x81\xa0\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_trailing_#.json
assert(not DecodeJson(' {"a":"b"}#{} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_object_with_trailing_garbage.json
assert(not DecodeJson(' {"a": true} "x" '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_object_followed_by_closing_object.json
assert(not DecodeJson(' {}} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_number_with_trailing_garbage.json
assert(not DecodeJson(' 2@ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_no_data.json
assert(not DecodeJson('  '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_end_array.json
assert(not DecodeJson(' ] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_double_array.json
assert(not DecodeJson(' [][] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_close_unopened_array.json
assert(not DecodeJson(' 1] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_array_with_extra_array_close.json
-- (added spaces between [[ and ]] so lua doesn't get confused)
assert(not DecodeJson([[
[ 1] ]   ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_array_trailing_garbage.json
assert(not DecodeJson(' [1]x '))
