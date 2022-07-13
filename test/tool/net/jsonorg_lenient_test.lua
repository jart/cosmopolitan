-- json.org says the following test cases should be
-- considered as invalid JSON, but ljson.c is lenient.
-- we run these tests anyway just to ensure that
-- no segfaults occurs while parsing these cases

-- [jart] we deviate from json.org because we don't care about colons
-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail19.json
assert(DecodeJson([[
{"Missing colon" null}
]]))
