-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail11.json
assert(nil == DecodeJson([[
{"Illegal expression": 1 + 2}
]]))
