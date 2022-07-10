-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: fail18.json
assert(pcall(DecodeJson, [[
[ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ [ "Too deep"] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] 
]]))
