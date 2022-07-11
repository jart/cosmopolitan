-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail11.json
assert(false == pcall(DecodeJson, [[
{"Illegal expression": 1 + 2}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail12.json
assert(false == pcall(DecodeJson, [[
{"Illegal invocation": alert()}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail13.json
assert(false == pcall(DecodeJson, [[
{"Numbers cannot have leading zeroes": 013}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail14.json
assert(false == pcall(DecodeJson, [[
{"Numbers cannot be hex": 0x14}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail15.json
assert(false == pcall(DecodeJson, [[
[ "Illegal backslash escape: \x15"] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail16.json
assert(false == pcall(DecodeJson, [[
[ \naked] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail17.json
assert(false == pcall(DecodeJson, [[
[ "Illegal backslash escape: \017"] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail22.json
assert(false == pcall(DecodeJson, [[
[ "Colon instead of comma": false] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail23.json
assert(false == pcall(DecodeJson, [[
[ "Bad value", truth] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail24.json
assert(false == pcall(DecodeJson, [[
[ 'single quote'] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail29.json
assert(false == pcall(DecodeJson, [[
[ 0e] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail2.json
assert(false == pcall(DecodeJson, [[
[ "Unclosed array"
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail30.json
assert(false == pcall(DecodeJson, [[
[ 0e+] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail31.json
assert(false == pcall(DecodeJson, [[
[ 0e+-1] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail32.json
assert(false == pcall(DecodeJson, [[
{"Comma instead if closing brace": true,
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail33.json
assert(false == pcall(DecodeJson, [[
[ "mismatch"}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail3.json
assert(false == pcall(DecodeJson, [[
{unquoted_key: "keys must be quoted"}
]]))
