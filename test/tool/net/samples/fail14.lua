-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail14.json
assert(false == pcall(DecodeJson, [[
{"Numbers cannot be hex": 0x14}
]]))
