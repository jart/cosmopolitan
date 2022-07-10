-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail2.json
assert(nil == DecodeJson([[
[ "Unclosed array"
]]))
