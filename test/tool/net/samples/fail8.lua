-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail8.json
assert(false == pcall(DecodeJson, [[
[ "Extra close"] ] 
]]))
