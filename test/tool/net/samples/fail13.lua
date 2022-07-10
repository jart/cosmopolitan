-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail13.json
assert(nil == DecodeJson([[
{"Numbers cannot have leading zeroes": 013}
]]))
