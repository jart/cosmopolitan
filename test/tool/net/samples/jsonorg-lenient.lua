-- json.org says the following test cases should be
-- considered as invalid JSON, but ljson.c is lenient.
-- we run these tests anyway just to ensure that
-- no segfaults occurs while parsing these cases

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail4.json
assert(DecodeJson([[
[ "extra comma",] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail5.json
assert(DecodeJson([[
[ "double extra comma",,] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail6.json
assert(DecodeJson([[
[    , "<-- missing value"] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail9.json
assert(DecodeJson([[
{"Extra comma": true,}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail19.json
assert(DecodeJson([[
{"Missing colon" null}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail20.json
assert(DecodeJson([[
{"Double colon":: null}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail25.json
assert(DecodeJson([[
[ "	tab	character	in	string	"] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail27.json
assert(DecodeJson([[
[ "line
break"] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail15.json
assert(DecodeJson([[
[ "Illegal backslash escape: \x15"] 
]]))
