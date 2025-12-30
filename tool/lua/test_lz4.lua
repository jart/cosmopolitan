local cosmo = require("cosmo")

-- Test Lz4Compress function exists
assert(type(cosmo.Lz4Compress) == "function", "Lz4Compress should be a function")
assert(type(cosmo.Lz4Decompress) == "function", "Lz4Decompress should be a function")

-- Test roundtrip with simple string
local original = "Hello, LZ4 compression!"
local compressed = cosmo.Lz4Compress(original)
assert(compressed ~= nil, "compression should succeed")
assert(#compressed > 0, "compressed data should not be empty")

local decompressed = cosmo.Lz4Decompress(compressed, #original)
assert(decompressed == original, "roundtrip should preserve data, got: " .. tostring(decompressed))

-- Test roundtrip with larger data
local large = string.rep("ABCD", 1000)
compressed = cosmo.Lz4Compress(large)
assert(compressed ~= nil, "large compression should succeed")
assert(#compressed < #large, "compression should reduce size for repetitive data")

decompressed = cosmo.Lz4Decompress(compressed, #large)
assert(decompressed == large, "large roundtrip should preserve data")

-- Test with acceleration parameter
compressed = cosmo.Lz4Compress(large, 1)
assert(compressed ~= nil, "compression with acceleration=1 should succeed")

local fast_compressed = cosmo.Lz4Compress(large, 10)
assert(fast_compressed ~= nil, "compression with acceleration=10 should succeed")

-- Test decompression with larger maxoutsize
decompressed = cosmo.Lz4Decompress(compressed, #large * 2)
assert(decompressed == large, "decompression with excess buffer should work")

-- Test error on invalid decompression
local result, err = cosmo.Lz4Decompress("not valid lz4 data", 100)
assert(result == nil, "invalid data should return nil")
assert(err ~= nil, "invalid data should return error message")

-- Test error on zero maxoutsize
result, err = cosmo.Lz4Decompress(compressed, 0)
assert(result == nil, "zero maxoutsize should return nil")
assert(err ~= nil, "zero maxoutsize should return error")

-- Test error on negative maxoutsize
result, err = cosmo.Lz4Decompress(compressed, -1)
assert(result == nil, "negative maxoutsize should return nil")
assert(err ~= nil, "negative maxoutsize should return error")

-- Test empty string compression
compressed = cosmo.Lz4Compress("")
assert(compressed ~= nil, "empty string compression should succeed")
decompressed = cosmo.Lz4Decompress(compressed, 10)  -- use small buffer, result will be ""
assert(decompressed == "", "empty string roundtrip should work, got: " .. tostring(decompressed))

-- Test binary data
local binary = "\x00\x01\x02\xff\xfe\xfd"
compressed = cosmo.Lz4Compress(binary)
assert(compressed ~= nil, "binary compression should succeed")
decompressed = cosmo.Lz4Decompress(compressed, #binary)
assert(decompressed == binary, "binary roundtrip should preserve data")

print("PASS")
