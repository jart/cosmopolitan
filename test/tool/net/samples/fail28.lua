-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail28.json
assert(false == pcall(DecodeJson, [[
[ "line\
break"] 
]]))
