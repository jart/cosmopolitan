-- Helper function to print test results
local function assert_equal(actual, expected, message)
    if actual ~= expected then
        error("FAIL: " .. message .. ": expected " .. tostring(expected) .. ", got " .. tostring(actual))
    end
end

local function assert_not_equal(actual, not_expected, message)
    if actual == not_expected then
        error(message .. ": did not expect " .. tostring(not_expected))
    end
end

-- Test RSA key pair generation
local function test_rsa_keypair_generation()
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    assert_equal(type(priv_key), "string", "Private key type")
    assert_equal(type(pub_key), "string", "Public key type")
end

-- Test ECDSA key pair generation
local function test_ecdsa_keypair_generation()
    local priv_key, pub_key = crypto.generatekeypair("ecdsa", "secp256r1")
    assert_equal(type(priv_key), "string", "Private key type")
    assert_equal(type(pub_key), "string", "Public key type")
end

-- Test RSA encryption and decryption
local function test_rsa_encryption_decryption()
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    assert(type(priv_key) == "string", "Private key type")
    assert(type(pub_key) == "string", "Public key type")
    local plaintext = "Hello, RSA!"
    local encrypted = crypto.encrypt("rsa", pub_key, plaintext)
    assert_equal(type(encrypted), "string", "Ciphertext type")
    local decrypted = crypto.decrypt("rsa", priv_key, encrypted)
    assert_equal(decrypted, plaintext, "Decrypted ciphertext matches plaintext")
end

-- Test RSA signing and verification
local function test_rsa_signing_verification()
    local priv_key, pub_key = crypto.generatekeypair("rsa", 2048)
    assert(type(priv_key) == "string", "Private key type")
    assert(type(pub_key) == "string", "Public key type")
    local message = "Sign this message"
    local signature = crypto.sign("rsa", priv_key, message, "sha256")
    assert_equal(type(signature), "string", "Signature type")
    local is_valid = crypto.verify("rsa", pub_key, message, signature, "sha256")
    assert_equal(is_valid, true, "Signature verification")
end

-- Test ECDSA signing and verification
local function test_ecdsa_signing_verification()
    local priv_key, pub_key = crypto.generatekeypair("ecdsa", "secp256r1")
    assert(type(priv_key) == "string", "Private key type")
    assert(type(pub_key) == "string", "Public key type")
    local message = "Sign this message with ECDSA"
    local signature = crypto.sign("ecdsa", priv_key, message, "sha256")
    assert_equal(type(signature), "string", "Signature type")
    local is_valid = crypto.verify("ecdsa", pub_key, message, signature, "sha256")
    assert_equal(is_valid, true, "Signature verification")
end

-- Test AES key generation
local function test_aes_key_generation()
    local key = crypto.generatekeypair('aes', 256) -- 256-bit key
    assert_equal(type(key), "string", "Key type")
    assert_equal(#key, 32, "Key length (256 bits)")
end

-- Test AES encryption and decryption (CBC mode)
local function test_aes_encryption_decryption()
    local key = crypto.generatekeypair('aes', 256) -- 256-bit key
    local plaintext = "Hello, AES CBC!"

    -- Encrypt without providing IV (should auto-generate IV)
    local encrypted, iv = crypto.encrypt("aes", key, plaintext, nil)
    assert_equal(type(encrypted), "string", "Ciphertext type")
    assert_equal(type(iv), "string", "IV type")

    -- Decrypt
    local decrypted = crypto.decrypt("aes", key, encrypted, iv)
    assert_equal(decrypted, plaintext, "Decrypted ciphertext matches plaintext")

    -- Encrypt with explicit IV
    local iv2 = GetRandomBytes(16)
    local encrypted2, iv_used = crypto.encrypt("aes", key, plaintext, iv2)
    assert_equal(type(encrypted2), "string", "Ciphertext type")
    assert_equal(iv_used, iv2, "IV match")

    local decrypted2 = crypto.decrypt("aes", key, encrypted2, iv2)
    assert_equal(decrypted2, plaintext, "Decrypted ciphertext matches plaintext")
end

-- Test AES encryption and decryption (CTR mode)
local function test_aes_encryption_decryption_ctr()
    local key = crypto.generatekeypair('aes', 256)
    local plaintext = "Hello, AES CTR!"

    -- Encrypt without providing IV (should auto-generate IV)
    local encrypted, iv = crypto.encrypt("aes", key, plaintext, nil, "ctr")
    assert_equal(type(encrypted), "string", "Ciphertext type")
    assert_equal(type(iv), "string", "IV type")

    -- Decrypt
    local decrypted = crypto.decrypt("aes", key, encrypted, iv, "ctr")
    assert_equal(decrypted, plaintext, "Decrypted ciphertext matches plaintext")

    -- Encrypt with explicit IV
    local iv2 = GetRandomBytes(16)
    local encrypted2, iv_used = crypto.encrypt("aes", key, plaintext, iv2, "ctr")
    assert_equal(type(encrypted2), "string", "Ciphertext type")
    assert_equal(iv_used, iv2, "IV match")

    local decrypted2 = crypto.decrypt("aes", key, encrypted2, iv2, "ctr")
    assert_equal(decrypted2, plaintext, "Decrypted ciphertext matches plaintext")
end

-- Test AES encryption and decryption (GCM mode)
local function test_aes_encryption_decryption_gcm()
    local key = crypto.generatekeypair('aes', 256)
    local plaintext = "Hello, AES GCM!"

    -- Encrypt without providing IV (should auto-generate IV)
    local encrypted, iv, tag = crypto.encrypt("aes", key, plaintext, nil, "gcm")
    assert_equal(#plaintext, #encrypted, "Ciphertext length matches plaintext")
    assert_equal(type(encrypted), "string", "Ciphertext type")
    assert_equal(type(iv), "string", "IV type")
    assert_equal(type(tag), "string", "Tag type")

    -- Decrypt
    local decrypted = crypto.decrypt("aes", key, encrypted, iv, "gcm", nil, tag)
    assert_equal(decrypted, plaintext, "Decrypted ciphertext matches plaintext")

    -- Encrypt with explicit IV
    local iv2 = GetRandomBytes(13) -- GCM IV/nonce can be 12-16 bytes, 12 is standard
    local encrypted2, iv_used, tag2 = crypto.encrypt("aes", key, plaintext, iv2, "gcm")
    assert_equal(type(encrypted2), "string", "Ciphertext type")
    assert_equal(iv_used, iv2, "IV match")
    assert_equal(type(tag2), "string", "Tag type")

    local decrypted2 = crypto.decrypt("aes", key, encrypted2, iv2, "gcm", nil, tag2)
    assert_equal(decrypted2, plaintext, "Decrypted ciphertext matches plaintext")
end

-- Test PemToJwk conversion
local function test_pem_to_jwk()
    local priv_key, pub_key = crypto.generatekeypair()
    local priv_jwk = crypto.convertPemToJwk(priv_key)
    assert_equal(type(priv_jwk), "table", "JWK type")
    assert_equal(priv_jwk.kty, "RSA", "kty is correct")

    local pub_jwk = crypto.convertPemToJwk(pub_key)
    assert_equal(type(pub_jwk), "table", "JWK type")
    assert_equal(pub_jwk.kty, "RSA", "kty is correct")

    -- Test ECDSA keys
    local priv_key, pub_key = crypto.generatekeypair('ecdsa')
    local priv_jwk = crypto.convertPemToJwk(priv_key)
    assert_equal(type(priv_jwk), "table", "JWK type")
    assert_equal(priv_jwk.kty, "EC", "kty is correct")

    local pub_jwk = crypto.convertPemToJwk(pub_key)
    assert_equal(type(pub_jwk), "table", "JWK type")
    assert_equal(pub_jwk.kty, "EC", "kty is correct")
end

-- Test CSR generation
local function test_csr_generation()
    local priv_key, _ = crypto.generatekeypair()
    local subject_name = "CN=example.com,O=Example Org,C=US"
    local san = "DNS:example.com, DNS:www.example.com, IP:192.168.1.1"
    assert_equal(type(priv_key), "string", "Private key type")

    local csr = crypto.generateCsr(priv_key, subject_name)
    assert_equal(type(csr), "string", "CSR generation with subject name")

    csr = crypto.generateCsr(priv_key, subject_name, san)
    assert_equal(type(csr), "string", "CSR generation with subject name and san")

    csr = crypto.generateCsr(priv_key, nil, san)
    assert_equal(type(csr), "string", "CSR generation with nil subject name and san")

    csr = crypto.generateCsr(priv_key, '', san)
    assert_equal(type(csr), "string", "CSR generation with empty subject name and san")

    -- These should fail
    csr = crypto.generateCsr(priv_key, '')
    assert_not_equal(type(csr), "string", "CSR generation with empty subject name and no san is rejected")

    csr = crypto.generateCsr(priv_key)
    assert_not_equal(type(csr), "string", "CSR generation with nil subject name and no san is rejected")
end

-- Run all tests
local function run_tests()
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
    EXIT = 0
    return EXIT
end

EXIT = 70
os.exit(run_tests())
