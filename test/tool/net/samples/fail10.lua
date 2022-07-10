-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail10.json
assert(nil == DecodeJson([[
{"Extra value after close": true} "misplaced quoted value"
]]))
