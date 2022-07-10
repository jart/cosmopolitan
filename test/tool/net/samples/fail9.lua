-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail9.json
assert(false == pcall(DecodeJson, [[
{"Extra comma": true,}
]]))
