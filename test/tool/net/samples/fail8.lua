-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail8.json
assert(nil == DecodeJson([[
[ "Extra close"] ] 
]]))
