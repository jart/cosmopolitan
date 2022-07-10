-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail3.json
assert(nil == DecodeJson([[
{unquoted_key: "keys must be quoted"}
]]))
