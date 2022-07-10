-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail12.json
assert(false == pcall(DecodeJson, [[
{"Illegal invocation": alert()}
]]))
