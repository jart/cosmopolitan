-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail26.json
assert(false == pcall(DecodeJson, [[
[ "tab\   character\   in\  string\  "] 
]]))
