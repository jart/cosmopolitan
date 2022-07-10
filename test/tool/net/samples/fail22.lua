-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail22.json
assert(false == pcall(DecodeJson, [[
[ "Colon instead of comma": false] 
]]))
