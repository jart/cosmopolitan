-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail21.json
assert(nil == DecodeJson([[
{"Comma instead of colon", null}
]]))
