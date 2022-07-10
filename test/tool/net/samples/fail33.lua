-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail33.json
assert(false == pcall(DecodeJson, [[
[ "mismatch"}
]]))
