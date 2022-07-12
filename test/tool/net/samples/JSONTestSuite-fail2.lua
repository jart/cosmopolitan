
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

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_whitespace_formfeed.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x0c\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_whitespace_U+2060_word_joiner.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\xe2\x81\xa0\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unicode-identifier.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \xc3\xa5 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_object.json
assert(false == pcall(DecodeJson, ' {"asd":"asd" '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_array_unfinished_true.json
assert(false == pcall(DecodeJson, ' [ false, tru '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_array_unfinished_false.json
assert(false == pcall(DecodeJson, ' [ true, fals '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_array_partial_null.json
assert(false == pcall(DecodeJson, ' [ false, nul '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_unclosed_array.json
assert(false == pcall(DecodeJson, ' [1 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_uescaped_LF_before_string.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x5c\x75\x30\x30\x30\x41\x22\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_single_star.json
assert(false == pcall(DecodeJson, ' * '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_single_eacute.json
assert(false == pcall(DecodeJson, ' é '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_open.json
assert(false == pcall(DecodeJson, [[ ["{["{["{["{ ]]))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_string_with_apostrophes.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x7b\x27\x61\x27 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_open_string.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x7b\x22\x61 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_open_array.json
assert(false == pcall(DecodeJson, ' {[ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_comma.json
assert(false == pcall(DecodeJson, ' {, '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object_close_array.json
assert(false == pcall(DecodeJson, ' {] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_object.json
assert(false == pcall(DecodeJson, ' { '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_string.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x22\x61\x22 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_open_string.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x22\x61 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_open_object.json
assert(false == pcall(DecodeJson, ' [{ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_comma.json
assert(false == pcall(DecodeJson, ' [, '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_open_array_apostrophe.json
assert(false == pcall(DecodeJson, ' [\' '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_object_with_comment.json
assert(false == pcall(DecodeJson, ' {"a":/*comment*/"b"} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_object_unclosed_no_value.json
assert(false == pcall(DecodeJson, ' {"": '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_null-byte-outside-string.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x00\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_lone-open-bracket.json
assert(false == pcall(DecodeJson, ' [ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_lone-invalid-utf-8.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \xe5 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_incomplete_UTF8_BOM.json
assert(false == pcall(DecodeJson, ' ï»{} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_comma_instead_of_closing_brace.json
assert(false == pcall(DecodeJson, ' {"x": true, '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_capitalized_True.json
assert(false == pcall(DecodeJson, ' [True] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_ascii-unicode-identifier.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x61\xc3\xa5 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_array_with_unclosed_string.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\x22\x61\x73\x64\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_angle_bracket_null.json
assert(false == pcall(DecodeJson, ' [<null>] '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_angle_bracket_..json
assert(false == pcall(DecodeJson, ' <.> '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_UTF8_BOM_no_data.json
assert(false == pcall(DecodeJson, ' ï»¿ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_structure_U+2060_word_joined.json
-- (converted to binary for safety)
assert(false == pcall(DecodeJson, ' \x5b\xe2\x81\xa0\x5d '))
