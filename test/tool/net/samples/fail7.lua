-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail7.json
assert(nil == DecodeJson([[
[ "Comma after the close"] ,
]]))
