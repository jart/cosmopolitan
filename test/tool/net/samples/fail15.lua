-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail15.json
assert(nil == DecodeJson([[
[ "Illegal backslash escape: \x15"] 
]]))
