-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail23.json
assert(nil == DecodeJson([[
[ "Bad value", truth] 
]]))
