-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail32.json
assert(false == pcall(DecodeJson, [[
{"Comma instead if closing brace": true,
]]))
