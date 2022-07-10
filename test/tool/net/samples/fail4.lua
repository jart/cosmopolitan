-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail4.json
assert(nil == DecodeJson([[
[ "extra comma",] 
]]))
