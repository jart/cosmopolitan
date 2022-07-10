-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail30.json
assert(false == pcall(DecodeJson, [[
[ 0e+] 
]]))
