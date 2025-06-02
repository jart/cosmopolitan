-- Helper function to print test results
local function assert_equal(actual, expected, message)
    if actual ~= expected then
        error(message .. ": expected " .. tostring(expected) .. ", got " .. tostring(actual))
    else
        print("PASS: " .. message)
    end
end

-- Test RSA key pair generation
local function test_rsa_keypair_generation()
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    assert_equal(type(priv_key), "string", "RSA private key generation")
    assert_equal(type(pub_key), "string", "RSA public key generation")
end

-- Test ECDSA key pair generation
local function test_ecdsa_keypair_generation()
    local priv_key, pub_key = crypto.generatekeypair("ecdsa", "secp256r1")
    assert_equal(type(priv_key), "string", "ECDSA private key generation")
    assert_equal(type(pub_key), "string", "ECDSA public key generation")
end

-- Test RSA encryption and decryption
local function test_rsa_encryption_decryption()
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    local message = "Hello, RSA!"
    local encrypted = crypto.encrypt("rsa", pub_key, message)
    assert_equal(type(encrypted), "string", "RSA encryption")
    local decrypted = crypto.decrypt("rsa", priv_key, encrypted)
    assert_equal(decrypted, message, "RSA decryption")
end

-- Test RSA signing and verification
local function test_rsa_signing_verification()
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    local message = "Sign this message"
    local signature = crypto.sign("rsa", priv_key, message, "sha256")
    assert_equal(type(signature), "string", "RSA signing")
    local is_valid = crypto.verify("rsa", pub_key, message, signature, "sha256")
    assert_equal(is_valid, true, "RSA signature verification")
end

-- Test ECDSA signing and verification
local function test_ecdsa_signing_verification()
    local priv_key, pub_key = crypto.generatekeypair("ecdsa", "secp256r1")
    local message = "Sign this message with ECDSA"
    local signature = crypto.sign("ecdsa", priv_key, message, "sha256")
    assert_equal(type(signature), "string", "ECDSA signing")
    local is_valid = crypto.verify("ecdsa", pub_key, message, signature, "sha256")
    assert_equal(is_valid, true, "ECDSA signature verification")
end

-- Test CSR generation
local function test_csr_generation()
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    local subject_name = "CN=example.com,O=Example Org,C=US"
    local csr = crypto.csrGenerate(priv_key, subject_name)
    assert_equal(type(csr), "string", "CSR generation")
end

-- Test PemToJwk conversion
local function test_pem_to_jwk()
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    local jwk = crypto.PemToJwk(pub_key)
    assert_equal(type(jwk), "table", "PEM to JWK conversion")
    assert_equal(jwk.kty, "RSA", "JWK key type")
end

-- Run all tests
local function run_tests()
    print("Running tests for lcrypto...")
    test_rsa_keypair_generation()
    test_ecdsa_keypair_generation()
    test_rsa_encryption_decryption()
    test_rsa_signing_verification()
    test_ecdsa_signing_verification()
    test_csr_generation()
    test_pem_to_jwk()
    print("All tests passed!")
end

run_tests()
