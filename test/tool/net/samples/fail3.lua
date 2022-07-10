-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail3.json
assert(false == pcall(DecodeJson, [[
{unquoted_key: "keys must be quoted"}
]]))
