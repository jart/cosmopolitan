-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail6.json
assert(false == pcall(DecodeJson, [[
[    , "<-- missing value"] 
]]))
