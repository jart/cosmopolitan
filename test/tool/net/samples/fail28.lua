-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail28.json
assert(nil == DecodeJson([[
[ "line\
break"] 
]]))
