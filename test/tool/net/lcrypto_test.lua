-- Helper function to print test results
local function assert_equal(actual, expected, plaintext)
    if actual ~= expected then
        error(plaintext .. ": expected " .. tostring(expected) .. ", got " .. tostring(actual))
    else
        print("PASS: " .. plaintext)
    end
end

local function assert_not_equal(actual, not_expected, plaintext)
    if actual == not_expected then
        error(plaintext .. ": did not expect " .. tostring(not_expected))
    else
        print("PASS: " .. plaintext)
    end
end

-- Test RSA key pair generation
local function test_rsa_keypair_generation()
    print('\27[1;7mTest RSA key pair generation                                            \27[0m')
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    assert_equal(type(priv_key), "string", "RSA private key generation")
    assert_equal(type(pub_key), "string", "RSA public key generation")
end

-- Test ECDSA key pair generation
local function test_ecdsa_keypair_generation()
    print('\n\27[1;7mTest ECDSA key pair generation                                          \27[0m')
    local priv_key, pub_key = crypto.generatekeypair("ecdsa", "secp256r1")
    assert_equal(type(priv_key), "string", "ECDSA private key generation")
    assert_equal(type(pub_key), "string", "ECDSA public key generation")
end

-- Test RSA encryption and decryption
local function test_rsa_encryption_decryption()
    print('\n\27[1;7mTest RSA encryption and decryption                                      \27[0m')
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    local plaintext = "Hello, RSA!"
    local encrypted = crypto.encrypt("rsa", pub_key, plaintext)
    assert_equal(type(encrypted), "string", "RSA encryption")
    local decrypted = crypto.decrypt("rsa", priv_key, encrypted)
    assert_equal(decrypted, plaintext, "RSA decryption")
end

-- Test RSA signing and verification
local function test_rsa_signing_verification()
    print('\n\27[1;7mTest RSA signing and verification                                       \27[0m')
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    local plaintext = "Sign this plaintext"
    local signature = crypto.sign("rsa", priv_key, plaintext, "sha256")
    assert_equal(type(signature), "string", "RSA signing")
    local is_valid = crypto.verify("rsa", pub_key, plaintext, signature, "sha256")
    assert_equal(is_valid, true, "RSA signature verification")
end

-- Test ECDSA signing and verification
local function test_ecdsa_signing_verification()
    print('\n\27[1;7mTest ECDSA signing and verification                                     \27[0m')
    local priv_key, pub_key = crypto.generatekeypair("ecdsa", "secp256r1")
    local plaintext = "Sign this plaintext with ECDSA"
    local signature = crypto.sign("ecdsa", priv_key, plaintext, "sha256")
    assert_equal(type(signature), "string", "ECDSA signing")
    local is_valid = crypto.verify("ecdsa", pub_key, plaintext, signature, "sha256")
    assert_equal(is_valid, true, "ECDSA signature verification")
end

-- Test AES key generation
local function test_aes_key_generation()
    print('\n\27[1;7mTest AES key generation                                                 \27[0m')
    local key = crypto.generatekeypair('aes', 256) -- 256-bit key
    assert_equal(type(key), "string", "AES key generation")
    assert_equal(#key, 32, "AES key length (256 bits)")
end

-- Test AES encryption and decryption (CBC mode)
local function test_aes_encryption_decryption()
    print('\n\27[1;7mTest AES encryption and decryption (CBC mode)                           \27[0m')
    local key = crypto.generatekeypair('aes',256) -- 256-bit key
    local plaintext = "Hello, AES CBC!"

    -- Encrypt without providing IV (should auto-generate IV)
    print('\27[1mAES encryption (auto IV)\27[0m')
    local encrypted, iv = crypto.encrypt("aes", key, plaintext, nil)
    assert_equal(type(encrypted), "string", "AES encryption (CBC, auto IV)")
    assert_equal(type(iv), "string", "AES IV (auto-generated)")

    -- Decrypt
    print('\n\27[1mAES decryption (auto IV)\27[0m')
    local decrypted = crypto.decrypt("aes", key, encrypted, iv)
    assert_equal(decrypted, plaintext, "AES decryption (CBC, auto IV)")

    -- Encrypt with explicit IV
    print('\n\27[1mAES encryption (explicit IV)\27[0m')
    local iv2 = GetRandomBytes(16)
    local encrypted2, iv_used = crypto.encrypt("aes", key, plaintext, iv2)
    assert_equal(type(encrypted2), "string", "AES encryption (CBC, explicit IV)")
    assert_equal(iv_used, iv2, "AES IV (explicit)")

    print('\n\27[1mAES decryption (explicit IV)\27[0m')
    local decrypted2 = crypto.decrypt("aes", key, encrypted2, iv2)
    assert_equal(decrypted2, plaintext, "AES decryption (CBC, explicit IV)")
end

-- Test AES encryption and decryption (CTR mode)
local function test_aes_encryption_decryption_ctr()
    print('\n\27[1;7mTest AES encryption and decryption (CTR mode)                           \27[0m')
    local key = crypto.generatekeypair('aes',256)
    local plaintext = "Hello, AES CTR!"

    -- Encrypt without providing IV (should auto-generate IV)
    print('\27[1mAES encryption (auto IV)\27[0m')
    local encrypted, iv = crypto.encrypt("aes", key, plaintext, nil, "ctr")
    assert_equal(type(encrypted), "string", "AES encryption (CTR, auto IV)")
    assert_equal(type(iv), "string", "AES IV (auto-generated, CTR)")

    -- Decrypt
    print('\n\27[1mAES decryption (auto IV)\27[0m')
    local decrypted = crypto.decrypt("aes", key, encrypted, iv, "ctr")
    assert_equal(decrypted, plaintext, "AES decryption (CTR, auto IV)")

    -- Encrypt with explicit IV
    print('\n\27[1mAES encryption (explicit IV)\27[0m')
    local iv2 = GetRandomBytes(16)
    local encrypted2, iv_used = crypto.encrypt("aes", key, plaintext, iv2, "ctr")
    assert_equal(type(encrypted2), "string", "AES encryption (CTR, explicit IV)")
    assert_equal(iv_used, iv2, "AES IV (explicit, CTR)")

    print('\n\27[1mAES decryption (explicit IV)\27[0m')
    local decrypted2 = crypto.decrypt("aes", key, encrypted2, iv2, "ctr")
    assert_equal(decrypted2, plaintext, "AES decryption (CTR, explicit IV)")
end

-- Test AES encryption and decryption (GCM mode)
local function test_aes_encryption_decryption_gcm()
    print('\n\27[1;7mTest AES encryption and decryption (GCM mode)                           \27[0m')
    local key = crypto.generatekeypair('aes',256)
    local plaintext = "Hello, AES GCM!"

    -- Encrypt without providing IV (should auto-generate IV)
    print('\27[1mAES encryption (auto IV)\27[0m')
    local encrypted, iv, tag = crypto.encrypt("aes", key, plaintext, nil, "gcm")
    assert_equal(type(encrypted), "string", "AES encryption (GCM, auto IV)")
    assert_equal(type(iv), "string", "AES IV (auto-generated, GCM)")
    assert_equal(type(tag), "string", "AES GCM tag (auto IV)")

    -- Decrypt
    print('\n\27[1mAES decryption (auto IV)\27[0m')
    local decrypted = crypto.decrypt("aes", key, encrypted, iv, "gcm", nil, tag)
    assert_equal(decrypted, plaintext, "AES decryption (GCM, auto IV)")

    -- Encrypt with explicit IV
    print('\n\27[1mAES encryption (explicit IV)\27[0m')
    local iv2 = GetRandomBytes(13) -- GCM IV/nonce can be 12-16 bytes, 12 is standard
    local encrypted2, iv_used, tag2 = crypto.encrypt("aes", key, plaintext, iv2, "gcm")
    assert_equal(type(encrypted2), "string", "AES encryption (GCM, explicit IV)")
    assert_equal(iv_used, iv2, "AES IV (explicit, GCM)")
    assert_equal(type(tag2), "string", "AES GCM tag (explicit IV)")

    print('\n\27[1mAES decryption (explicit IV)\27[0m')
    local decrypted2 = crypto.decrypt("aes", key, encrypted2, iv2, "gcm", nil, tag2)
    assert_equal(decrypted2, plaintext, "AES decryption (GCM, explicit IV)")
end

-- Test PemToJwk conversion
local function test_pem_to_jwk()
    print('\n\27[1;7mTest PEM to JWK conversion                                              \27[0m')
    local priv_key, pub_key = crypto.generatekeypair()
    print('\27[1mRSA Private key to JWK conversion\27[0m')
    local priv_jwk = crypto.convertPemToJwk(priv_key)
    assert_equal(type(priv_jwk), "table", "PEM to JWK conversion")
    assert_equal(priv_jwk.kty, "RSA", "JWK key type")

    print('\n\27[1mRSA Public key to JWK conversion\27[0m')
    local pub_jwk = crypto.convertPemToJwk(pub_key)
    assert_equal(type(pub_jwk), "table", "PEM to JWK conversion")
    assert_equal(pub_jwk.kty, "RSA", "JWK key type")

    -- Test ECDSA keys
    local priv_key, pub_key = crypto.generatekeypair('ecdsa')
    print('\n\27[1mECDSA Private key to JWK conversion\27[0m')
    local priv_jwk = crypto.convertPemToJwk(priv_key)
    assert_equal(type(priv_jwk), "table", "PEM to JWK conversion")
    assert_equal(priv_jwk.kty, "EC", "JWK key type")

    print('\n\27[1mECDSA Public key to JWK conversion\27[0m')
    local pub_jwk = crypto.convertPemToJwk(pub_key)
    assert_equal(type(pub_jwk), "table", "PEM to JWK conversion")
    assert_equal(pub_jwk.kty, "EC", "JWK key type")
end

-- Test CSR generation
local function test_csr_generation()
    print('\n\27[1;7mTest CSR generation                                                     \27[0m')
    local priv_key, _ = crypto.generatekeypair()
    local subject_name = "CN=example.com,O=Example Org,C=US"
    local san = "DNS:example.com, DNS:www.example.com, IP:192.168.1.1"

    local csr = crypto.GenerateCsr(priv_key, subject_name)
    assert_equal(type(csr), "string", "CSR generation with subject name")

    csr = crypto.GenerateCsr(priv_key, subject_name, san)
    assert_equal(type(csr), "string", "CSR generation with subject name and san")

    csr = crypto.GenerateCsr(priv_key, nil, san)
    assert_equal(type(csr), "string", "CSR generation with nil subject name and san")

    csr = crypto.GenerateCsr(priv_key, '', san)
    assert_equal(type(csr), "string", "CSR generation with empty subject name and san")

    -- These should fail
    csr = crypto.GenerateCsr(priv_key, '')
    assert_not_equal(type(csr), "string", "CSR generation with empty subject name and no san is rejected")

    csr = crypto.GenerateCsr(priv_key)
    assert_not_equal(type(csr), "string", "CSR generation with nil subject name and no san is rejected")
end

-- Run all tests
local function run_tests()
    print("Running tests for lcrypto...")
    test_rsa_keypair_generation()
    test_rsa_signing_verification()
    test_rsa_encryption_decryption()
    test_ecdsa_keypair_generation()
    test_ecdsa_signing_verification()
    test_aes_key_generation()
    test_aes_encryption_decryption()
    test_aes_encryption_decryption_ctr()
    test_aes_encryption_decryption_gcm()
    test_pem_to_jwk()
    test_csr_generation()
    print('')
    print("All tests passed!")
    EXIT=0
    return EXIT
end

EXIT=70
os.exit(run_tests())
