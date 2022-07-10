-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail1.json
assert(pcall(DecodeJson, [[
"A JSON payload should be an object or array, not a string."
]]))
