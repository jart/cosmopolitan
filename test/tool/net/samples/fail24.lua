-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail24.json
assert(nil == DecodeJson([[
[ 'single quote'] 
]]))
