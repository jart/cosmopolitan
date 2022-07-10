-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail20.json
assert(false == pcall(DecodeJson, [[
{"Double colon":: null}
]]))
