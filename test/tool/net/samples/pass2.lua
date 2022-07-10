-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: pass2.json
assert(pcall(DecodeJson, [[
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ "Not too deep"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] 
]]))
