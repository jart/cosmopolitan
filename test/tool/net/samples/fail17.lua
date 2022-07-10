-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail17.json
assert(nil == DecodeJson([[
[ "Illegal backslash escape: \017"] 
]]))
