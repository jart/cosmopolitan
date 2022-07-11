-- json.org says the following test cases should be
-- considered as invalid JSON, but ljson.c is lenient.
-- we run these tests anyway just to ensure that
-- no segfaults occurs while parsing these cases

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail4.json
assert(nil ~= pcall(DecodeJson, [[
[ "extra comma",] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail5.json
assert(nil ~= pcall(DecodeJson, [[
[ "double extra comma",,] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail6.json
assert(nil ~= pcall(DecodeJson, [[
[    , "<-- missing value"] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail7.json
assert(nil ~= pcall(DecodeJson, [[
[ "Comma after the close"] ,
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail8.json
assert(nil ~= pcall(DecodeJson, [[
[ "Extra close"] ] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail9.json
assert(nil ~= pcall(DecodeJson, [[
{"Extra comma": true,}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail19.json
assert(nil ~= pcall(DecodeJson, [[
{"Missing colon" null}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail20.json
assert(nil ~= pcall(DecodeJson, [[
{"Double colon":: null}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail21.json
assert(nil ~= pcall(DecodeJson, [[
{"Comma instead of colon", null}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail10.json
assert(nil ~= pcall(DecodeJson, [[
{"Extra value after close": true} "misplaced quoted value"
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail25.json
assert(nil ~= pcall(DecodeJson, [[
[ "	tab	character	in	string	"] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail26.json
assert(nil ~= pcall(DecodeJson, [[
[ "tab\   character\   in\  string\  "] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail27.json
assert(nil ~= pcall(DecodeJson, [[
[ "line
break"] 
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail28.json
assert(nil ~= pcall(DecodeJson, [[
[ "line\
break"] 
]]))
