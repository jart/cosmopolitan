-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail13.json
assert(false == pcall(DecodeJson, [[
{"Numbers cannot have leading zeroes": 013}
]]))
