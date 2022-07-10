-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail19.json
assert(nil == DecodeJson([[
{"Missing colon" null}
]]))
