-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail27.json
assert(nil == DecodeJson([[
[ "line
break"] 
]]))
