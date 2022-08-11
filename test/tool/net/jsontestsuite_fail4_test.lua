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

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_start_escape_unclosed.json
assert(not DecodeJson(" [\"\\ "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_single_string_no_double_quotes.json
assert(not DecodeJson(" abc "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_single_quote.json
assert(not DecodeJson(" [\'single quote\'] "))

-- disallow escape code outside string
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_no_quotes_with_bad_escape.json
assert(not DecodeJson(" [\\n] "))

-- disallow unicode escape outside string
-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_leading_uescaped_thinspace.json
assert(not DecodeJson(" [\\u0020\"asd\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_incomplete_surrogate_escape_invalid.json
assert(not DecodeJson(" [\"\\uD800\\uD800\\x\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_incomplete_escape.json
assert(not DecodeJson(" [\"\\\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_escaped_backslash_bad.json
assert(not DecodeJson(" [\"\\\\\\\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_accentuated_char_no_quotes.json
assert(not DecodeJson(" [é] "))
assert(DecodeJson(" [\"é\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_1_surrogate_then_escape.json
assert(not DecodeJson(" [\"\\uD800\\\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_with_single_string.json
assert(not DecodeJson(" { \"foo\" : \"bar\", \"a\" } "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_unterminated-value.json
assert(not DecodeJson(' {"a":"a '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_unquoted_key.json
assert(not DecodeJson(' {a: "b"} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_single_quote.json
assert(not DecodeJson(' {\'a\':0} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_repeated_null_null.json
assert(not DecodeJson(' {null:null,null:null} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_non_string_key_but_huge_number_instead.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x7b\x39\x39\x39\x39\x45\x39\x39\x39\x39\x3a\x31\x7d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_non_string_key.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x7b\x31\x3a\x31\x7d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_array_comma_after_close.json
assert(not DecodeJson(' [""], '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_backslash_00.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x00\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_escaped_emoji.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\xf0\x9f\x8c\x80\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_escaped_ctrl_char_tab.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x09\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_incomplete_surrogate.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x75\x44\x38\x33\x34\x5c\x75\x44\x64\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_incomplete_escaped_character.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x75\x30\x30\x41\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_single_doublequote.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x22 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_invalid_utf8_after_escape.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\xe5\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_invalid_unicode_escape.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x75\x71\x71\x71\x71\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_invalid_backslash_esc.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x61\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_invalid-utf-8-in-escape.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x75\xe5\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_1_surrogate_then_escape_u1x.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x75\x44\x38\x30\x30\x5c\x75\x31\x78\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_1_surrogate_then_escape_u1.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x75\x44\x38\x30\x30\x5c\x75\x31\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_1_surrogate_then_escape_u.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x5b\x22\x5c\x75\x44\x38\x30\x30\x5c\x75\x22\x5d '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_unicode_CapitalU.json
-- (converted to binary for safety)
assert(not DecodeJson(' \x22\x5c\x55\x41\x36\x36\x44\x22 '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_with_trailing_garbage.json
-- (converted to binary for safety)
assert(not DecodeJson(" \"\"x "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_single_space.json
assert(not DecodeJson('  '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_with_trailing_garbage.json
assert(not DecodeJson(' {"a":"b"}# '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_trailing_comment_slash_open_incomplete.json
assert(not DecodeJson(' {"a":"b"}/ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_trailing_comment_slash_open.json
assert(not DecodeJson(' {"a":"b"}// '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_trailing_comment_open.json
assert(not DecodeJson(' {"a":"b"}/**// '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_trailing_comment.json
assert(not DecodeJson(' {"a":"b"}/**/ '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_unescaped_ctrl_char.json
assert(not DecodeJson(" [\"a\x00a\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_escape_x.json
assert(not DecodeJson(" [\"a\\x00a\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_unescaped_tab.json
assert(not DecodeJson(" [\"\t\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_string_unescaped_newline.json
assert(not DecodeJson(" [\"new\nline\"] "))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_two_commas_in_a_row.json
assert(not DecodeJson(' {"a":"b",,"c":"d"} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_trailing_comma.json
assert(not DecodeJson(' {"id":0,} '))

-- https://github.com/nst/JSONTestSuite/tree/d64aefb55228d9584d3e5b2433f720ea8fd00c82/test_parsing/n_object_several_trailing_commas.json
assert(not DecodeJson(' {"id":0,,,,,} '))
