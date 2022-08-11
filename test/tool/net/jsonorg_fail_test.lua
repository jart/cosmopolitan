assert(unix.pledge("stdio"))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail11.json
assert(not DecodeJson([[
{"Illegal expression": 1 + 2}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail12.json
assert(not DecodeJson([[
{"Illegal invocation": alert()}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail13.json
assert(not DecodeJson([[
{"Numbers cannot have leading zeroes": 013}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail14.json
assert(not DecodeJson([[
{"Numbers cannot be hex": 0x14}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail16.json
assert(not DecodeJson([[
[ \naked]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail17.json
assert(not DecodeJson([[
[ "Illegal backslash escape: \017"]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail22.json
assert(not DecodeJson([[
[ "Colon instead of comma": false]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail23.json
assert(not DecodeJson([[
[ "Bad value", truth]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail24.json
assert(not DecodeJson([[
[ 'single quote']
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail29.json
assert(not DecodeJson([[
[ 0e]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail2.json
assert(not DecodeJson([[
[ "Unclosed array"
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail30.json
assert(not DecodeJson([[
[ 0e+]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail31.json
assert(not DecodeJson([[
[ 0e+-1]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail32.json
assert(not DecodeJson([[
{"Comma instead if closing brace": true,
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail33.json
assert(not DecodeJson([[
[ "mismatch"}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail3.json
assert(not DecodeJson([[
{unquoted_key: "keys must be quoted"}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail21.json
assert(not DecodeJson([[
{"Comma instead of colon", null}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail8.json
assert(not DecodeJson([[
[ "Extra close"] ]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail7.json
assert(not DecodeJson([[
[ "Comma after the close"] ,
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail10.json
assert(not DecodeJson([[
{"Extra value after close": true} "misplaced quoted value"
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail26.json
assert(not DecodeJson([[
[ "tab\   character\   in\  string\  "]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail28.json
assert(not DecodeJson([[
[ "line\
break"]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail4.json
assert(not DecodeJson([[
[ "extra comma",]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail5.json
assert(not DecodeJson([[
[ "double extra comma",,]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail6.json
assert(not DecodeJson([[
[    , "<-- missing value"]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail9.json
assert(not DecodeJson([[
{"Extra comma": true,}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail20.json
assert(not DecodeJson([[
{"Double colon":: null}
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail25.json
assert(not DecodeJson([[
[ "	tab	character	in	string	"]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail27.json
assert(not DecodeJson([[
[ "line
break"]
]]))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail15.json
assert(not DecodeJson(' ["Illegal backslash escape: \x15"] '))

-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail19.json
assert(not DecodeJson([[
{"Missing colon" null}
]]))
