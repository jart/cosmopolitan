-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail11.json
assert(false == pcall(DecodeJson, [[
{"Illegal expression": 1 + 2}
]]))
