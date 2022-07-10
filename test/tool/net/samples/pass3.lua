-- https://www.json.org/JSON_checker/test.zip
-- JSON parsing sample test case: pass3.json
assert(pcall(DecodeJson, [[
{
    "JSON Test Pattern pass3": {
        "The outermost value": "must be an object or array.",
        "In this test": "It is an object."
    }
}

]]))
