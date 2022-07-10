-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail5.json
assert(nil == DecodeJson([[
[ "double extra comma",,] 
]]))
