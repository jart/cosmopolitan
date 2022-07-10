-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail24.json
assert(false == pcall(DecodeJson, [[
[ 'single quote'] 
]]))
