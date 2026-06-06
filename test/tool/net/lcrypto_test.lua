-- Test RSA key pair generation
local function test_rsa_keypair_generation()
    local priv_key, pub_key = crypto.generateKeyPair("rsa", 2048)
    assert(type(priv_key) == "string", "Private key type")
    assert(type(pub_key) == "string", "Public key type")
end

-- Test ECDSA key pair generation
local function test_ecdsa_keypair_generation()
    local priv_key, pub_key = crypto.generateKeyPair("ecdsa", "secp256r1")
    assert(type(priv_key) == "string", "Private key type")
    assert(type(pub_key) == "string", "Public key type")
end

-- Test RSA encryption and decryption
local function test_rsa_encryption_decryption()
    local priv_key, pub_key = crypto.generateKeyPair("rsa", 2048)
    assert(type(priv_key) == "string", "Private key type")
    assert(type(pub_key) == "string", "Public key type")

    local plaintext = "Hello, RSA!"
    local ciphertext = crypto.encrypt("rsa", pub_key, plaintext)
    assert(type(ciphertext) == "string", "Ciphertext type")

    local decrypted_plaintext = crypto.decrypt("rsa", priv_key, ciphertext)
    assert(decrypted_plaintext == plaintext, "Decrypted ciphertext matches plaintext")
end

-- Test RSA signing and verification
local function test_rsa_signing_verification()
    local priv_key, pub_key = crypto.generateKeyPair("rsa", 2048)
    assert(type(priv_key) == "string", "Private key type")
    assert(type(pub_key) == "string", "Public key type")

    local message = "Sign this message"
    local signature = crypto.sign("rsa", priv_key, message, "sha256")
    assert(type(signature) == "string", "Signature type")

    local is_valid = crypto.verify("rsa", pub_key, message, signature, "sha256")
    assert(is_valid == true, "Signature verification")
end

-- Test RSA-PSS signing and verification
local function test_rsapss_signing_verification()
    local priv_key, pub_key = crypto.generateKeyPair("rsa", 2048)
    assert(type(priv_key) == "string", "Private key type")
    assert(type(pub_key) == "string", "Public key type")

    Log(kLogVerbose," - Testing RSA-PSS signing")
    local message = "Sign this message with RSA-PSS"
    local signature = crypto.sign("rsapss", priv_key, message, "sha256")
    assert(type(signature) == "string", "Signature type")

    Log(kLogVerbose," - Testing RSA-PSS verification")
    local is_valid = crypto.verify("rsapss", pub_key, message, signature, "sha256")
    assert(is_valid == true, "RSA-PSS Signature verification")

    -- Test with different hash algorithm
    Log(kLogVerbose," - Testing RSA-PSS with different hash algorithms")
    signature = crypto.sign("rsapss", priv_key, message, "sha384")
    assert(type(signature) == "string", "SHA-384 Signature type")
    is_valid = crypto.verify("rsapss", pub_key, message, signature, "sha384")
    assert(is_valid == true, "RSA-PSS SHA-384 Signature verification")

    Log(kLogVerbose," - Testing RSA-PSS with SHA-512")
    signature = crypto.sign("rsapss", priv_key, message, "sha512")
    assert(type(signature) == "string", "SHA-512 Signature type")
    is_valid = crypto.verify("rsapss", pub_key, message, signature, "sha512")
    assert(is_valid == true, "RSA-PSS SHA-512 Signature verification")
end

-- Test ECDSA signing and verification
local function test_ecdsa_signing_verification()
    local priv_key, pub_key = crypto.generateKeyPair("ecdsa", "secp256r1")
    assert(type(priv_key) == "string", "Private key type")
    assert(type(pub_key) == "string", "Public key type")

    local message = "Sign this message with ECDSA"
    local signature = crypto.sign("ecdsa", priv_key, message, "sha256")
    assert(type(signature) == "string", "Signature type")

    local is_valid = crypto.verify("ecdsa", pub_key, message, signature, "sha256")
    assert(is_valid == true, "Signature verification")
end

-- Test AES key generation
local function test_aes_key_generation()
    local key = crypto.generateKeyPair('aes', 256) -- 256-bit key
    assert(type(key) == "string", "Key type")
    assert(#key == 32, "Key length (256 bits)")
end

-- Test AES encryption and decryption (CBC mode)
local function test_aes_encryption_decryption_cbc()
    local key = crypto.generateKeyPair('aes', 256) -- 256-bit key
    local plaintext = "Hello, AES CBC!"

    -- Encrypt without providing IV (should auto-generate IV)
    local ciphertext, iv = crypto.encrypt("aes", key, plaintext, nil)
    assert(type(ciphertext) == "string", "Ciphertext type")
    assert(type(iv) == "string", "IV type")

    -- Decrypt
    local decrypted_plaintext = crypto.decrypt("aes", key, ciphertext, { mode = "cbc", iv = iv })
    assert(decrypted_plaintext == plaintext, "Decrypted ciphertext matches plaintext")

    -- Encrypt with explicit IV
    local iv2 = GetRandomBytes(16)
    local ciphertext2, iv_used = crypto.encrypt("aes", key, plaintext, { mode = "cbc", iv = iv2 })
    assert(type(ciphertext2) == "string", "Ciphertext type")
    assert(iv_used == iv2, "IV match")

    local decrypted_plaintext2 = crypto.decrypt("aes", key, ciphertext2, { mode = "cbc", iv = iv2 })
    assert(decrypted_plaintext2 == plaintext, "Decrypted ciphertext matches plaintext")
end

-- Test AES encryption and decryption (CTR mode)
local function test_aes_encryption_decryption_ctr()
    local key = crypto.generateKeyPair('aes', 256)
    local plaintext = "Hello, AES CTR!"

    -- Encrypt without providing IV (should auto-generate IV)
    local ciphertext, iv = crypto.encrypt("aes", key, plaintext, { mode = "ctr" })
    assert(type(ciphertext) == "string", "Ciphertext type")
    assert(type(iv) == "string", "IV type")

    -- Decrypt
    local decrypted_plaintext = crypto.decrypt("aes", key, ciphertext, { mode = "ctr", iv = iv })
    assert(decrypted_plaintext == plaintext, "Decrypted ciphertext matches plaintext")

    -- Encrypt with explicit IV
    local iv2 = GetRandomBytes(16)
    local ciphertext2, iv_used = crypto.encrypt("aes", key, plaintext, { mode = "ctr", iv = iv2 })
    assert(type(ciphertext2) == "string", "Ciphertext type")
    assert(iv_used == iv2, "IV match")

    local decrypted_plaintext2 = crypto.decrypt("aes", key, ciphertext2, { mode = "ctr", iv = iv2 })
    assert(decrypted_plaintext2 == plaintext, "Decrypted ciphertext matches plaintext")
end

-- Test AES encryption and decryption (GCM mode)
local function test_aes_encryption_decryption_gcm()
    local key = crypto.generateKeyPair('aes', 256)
    assert(type(key) == "string", "key type")
    local plaintext = "Hello, AES GCM!"

    -- Encrypt without providing IV (should auto-generate IV)
    local ciphertext, iv, tag = crypto.encrypt("aes", key, plaintext, { mode = "gcm" })
    assert(#plaintext == #ciphertext, "Ciphertext length matches plaintext")
    assert(type(ciphertext) == "string", "Ciphertext type")
    assert(type(iv) == "string", "IV type")
    assert(type(tag) == "string", "Tag type")

    -- Decrypt
    local decrypted_plaintext = crypto.decrypt("aes", key, ciphertext, { mode = "gcm", iv = iv, tag = tag })
    assert(decrypted_plaintext == plaintext, "Decrypted ciphertext matches plaintext")

    -- Encrypt with explicit IV
    local iv2 = GetRandomBytes(13) -- GCM IV/nonce can be 12-16 bytes, 12 is standard
    local ciphertext2, iv_used, tag2 = crypto.encrypt("aes", key, plaintext, { mode = "gcm", iv = iv2 })
    assert(type(ciphertext2) == "string", "Ciphertext type")
    assert(iv_used == iv2, "IV match")
    assert(type(tag2) == "string", "Tag type")

    local decrypted_plaintext2 = crypto.decrypt("aes", key, ciphertext2, { mode = "gcm", iv = iv2, tag = tag2 })
    assert(decrypted_plaintext2 == plaintext, "Decrypted ciphertext matches plaintext")
end

-- Test PemToJwk conversion
local function test_pem_to_jwk()
    local priv_key, pub_key = crypto.generateKeyPair()
    local priv_jwk = crypto.convertPemToJwk(priv_key)
    assert(type(priv_jwk) == "table", "JWK type")
    assert(priv_jwk.kty == "RSA", "kty is correct")

    local pub_jwk = crypto.convertPemToJwk(pub_key)
    assert(type(pub_jwk) == "table", "JWK type")
    assert(pub_jwk.kty == "RSA", "kty is correct")

    -- Test ECDSA keys
    priv_key, pub_key = crypto.generateKeyPair('ecdsa')
    priv_jwk = crypto.convertPemToJwk(priv_key)
    assert(type(priv_jwk) == "table", "JWK type")
    assert(priv_jwk.kty == "EC", "kty is correct")

    pub_jwk = crypto.convertPemToJwk(pub_key)
    assert(type(pub_jwk) == "table", "JWK type")
    assert(pub_jwk.kty == "EC", "kty is correct")
end

-- Test JwkToPem conversion
local function test_jwk_to_pem()
    local priv_key, pub_key = crypto.generateKeyPair()
    local priv_jwk = crypto.convertPemToJwk(priv_key)
    local pub_jwk = crypto.convertPemToJwk(pub_key)

    local priv_pem = crypto.convertJwkToPem(priv_jwk)
    local pub_pem = crypto.convertJwkToPem(pub_jwk)
    assert(type(priv_pem) == "string", "Private PEM type")

    -- Roundtrip
    assert(priv_key == priv_pem, "Private PEM matches original RSA key")
    assert(pub_key == pub_pem, "Public PEM matches original RSA key")

    pub_pem = crypto.convertJwkToPem(pub_jwk)
    assert(type(pub_pem) == "string", "Public PEM type")

    -- Test ECDSA keys
    priv_key, pub_key = crypto.generateKeyPair('ecdsa')
    priv_jwk = crypto.convertPemToJwk(priv_key)
    pub_jwk = crypto.convertPemToJwk(pub_key)

    priv_pem = crypto.convertJwkToPem(priv_jwk)
    pub_pem = crypto.convertJwkToPem(pub_jwk)
    assert(type(priv_pem) == "string", "Private PEM type for ECDSA")

    -- Roundtrip
    assert(priv_key == priv_pem, "Private PEM matches original ECDSA key")
    assert(pub_key == pub_pem, "Public PEM matches original ECDSA key")

    pub_pem = crypto.convertJwkToPem(pub_jwk)
    assert(type(pub_pem) == "string", "Public PEM type for ECDSA")
end

-- Test CSR generation
local function test_csr_generation()
    local priv_key, _ = crypto.generateKeyPair()
    local subject_name = "CN=example.com,O=Example Org,C=US"
    local san = "DNS:example.com, DNS:www.example.com, IP:192.168.1.1"
    assert(type(priv_key) == "string", "Private key type")

    local csr = crypto.generateCsr(priv_key, subject_name)
    assert(type(csr) == "string", "CSR generation with subject name")

    csr = crypto.generateCsr(priv_key, subject_name, san)
    assert(type(csr) == "string", "CSR generation with subject name and san")

    csr = crypto.generateCsr(priv_key, nil, san)
    assert(type(csr) == "string", "CSR generation with nil subject name and san")

    csr = crypto.generateCsr(priv_key, '', san)
    assert(type(csr) == "string", "CSR generation with empty subject name and san")

    -- These should fail
    csr = crypto.generateCsr(priv_key, '')
    assert(type(csr) ~= "string", "CSR generation with empty subject name and no san is rejected")

    csr = crypto.generateCsr(priv_key)
    assert(type(csr) ~= "string", "CSR generation with nil subject name and no san is rejected")
end

-- Test various hash algorithms
local function test_hash_algorithms()
    local priv_key, pub_key = crypto.generateKeyPair("rsa", 2048)
    local message = "Test message for hash algorithms"

    -- Test different hash algorithms for RSA signatures
    local hash_algorithms = { "sha256", "sha384", "sha512" }
    for _, hash in ipairs(hash_algorithms) do
        local signature = crypto.sign("rsa", priv_key, message, hash)
        assert(type(signature) == "string", "RSA signature with " .. hash)
        local is_valid = crypto.verify("rsa", pub_key, message, signature, hash)
        assert(is_valid == true, "RSA verification with " .. hash)

        -- Test with RSA-PSS
        local signature_pss = crypto.sign("rsapss", priv_key, message, hash)
        assert(type(signature_pss) == "string", "RSA-PSS signature with " .. hash)
        local is_valid_pss = crypto.verify("rsapss", pub_key, message, signature_pss, hash)
        assert(is_valid_pss == true, "RSA-PSS verification with " .. hash)
    end

    -- Test ECDSA with different hash algorithms
    local ec_priv_key, ec_pub_key = crypto.generateKeyPair("ecdsa", "secp256r1")
    for _, hash in ipairs(hash_algorithms) do
        local signature = crypto.sign("ecdsa", ec_priv_key, message, hash)
        assert(type(signature) == "string", "ECDSA signature with " .. hash)

        local is_valid = crypto.verify("ecdsa", ec_pub_key, message, signature, hash)
        assert(is_valid == true, "ECDSA verification with " .. hash)
    end
end

-- Test negative cases for hash algorithms
local function test_negative_hash_algorithms()
    local priv_key, pub_key = crypto.generateKeyPair()
    local message = "Test message for hash algorithms"

    -- Test with invalid hash algorithm
    local ok = pcall(function() return crypto.sign("rsa", priv_key, message, "invalid-hash") end)
    assert(ok == false, "Sign with invalid hash should fail")

    -- Test with nil hash algorithm (should default to SHA-256)
    local signature = crypto.sign("rsa", priv_key, message)
    assert(type(signature) == "string", "Sign with nil hash")

    local is_valid = crypto.verify("rsa", pub_key, message, signature)
    assert(is_valid == true, "Verify with nil hash")
end

-- Negative tests for crypto functions
local function test_negative_keypair_generation()
    -- Invalid algorithm
    local ok = pcall(function() return crypto.generateKeyPair('invalidalg', 2048) end)
    assert(ok == false, "generatekeypair with invalid algorithm should fail")
    -- Invalid RSA key size
    local pk, _ = crypto.generateKeyPair('rsa', 123)
    assert(pk == nil, "generatekeypair with invalid RSA size should fail")
    -- Invalid ECDSA curve
    pk, _ = crypto.generateKeyPair('ecdsa', 'invalidcurve')
    assert(pk == nil, "generatekeypair with invalid ECDSA curve should fail")
end

local function test_negative_encrypt_decrypt()
    local priv_key, pub_key = crypto.generateKeyPair('rsa', 2048)
    -- Encrypt with invalid algorithm
    local ok, ciphertext = pcall(function() return crypto.encrypt('invalidalg', pub_key, 'data') end)
    assert(ok == false, "RSA encrypt with invalid algorithm should fail")

    -- Decrypt with invalid algorithm
    ok, _ = pcall(function() return crypto.decrypt('invalidalg', priv_key, 'data') end)
    assert(ok == false, "RSA decrypt with invalid algorithm should fail")

    -- Encrypt with invalid key
    ciphertext = crypto.encrypt('rsa', 'notakey', 'data')
    assert(ciphertext == nil, "RSA encrypt with invalid key should fail")

    -- Decrypt with invalid key
    local retval = crypto.decrypt('rsa', 'notakey', 'data')
    assert(retval == nil, "RSA decrypt with invalid key should fail")

    -- AES: invalid IV length
    local key = crypto.generateKeyPair('aes', 256)
    ciphertext = crypto.encrypt('aes', key, 'data', { mode = "cbc", iv = "tooShortIV" })
    assert(ciphertext == nil, "AES encrypt with short IV should fail")

    retval = crypto.decrypt('aes', key, 'data', { mode = "cbc", iv = "tooShortIV" })
    assert(retval == nil, "AES decrypt with short IV should fail")
end

local function test_negative_sign_verify()
    local priv_key, pub_key = crypto.generateKeyPair('rsa', 2048)
    -- Sign with invalid algorithm
    local ok = pcall(function() return crypto.sign('invalidalg', priv_key, 'msg', 'sha256') end)
    assert(ok == false, "RSA sign with invalid algorithm should fail")

    -- Verify with invalid algorithm
    ok = pcall(function() return crypto.verify('invalidalg', pub_key, 'msg', 'sig', 'sha256') end)
    assert(ok == false, "RSA verify with invalid algorithm should fail")

    -- Sign with invalid key
    ok = pcall(function() return crypto.sign('rsa', 'notakey', 'msg', 'sha256') end)
    assert(ok == false, "RSA sign with invalid key should fail")

    -- Verify with invalid key
    local verified = crypto.verify('rsa', 'notakey', 'msg', 'sig', 'sha256')
    assert(verified == false, "verify with invalid key should fail")

    -- Verify with wrong signature (should return false, not error)
    local badsig = 'thisisnotavalidsignature'
    local result = crypto.verify('rsa', pub_key, 'msg', badsig, 'sha256')
    assert(result == false, "RSA verify with wrong signature should return false")
end

local function test_negative_pem_jwk_conversion()
    -- Invalid PEM
    local ok = pcall(function() return crypto.convertPemToJwk('notapem') end)
    assert(ok == false, "convertPemToJwk with invalid PEM should fail")

    -- Invalid JWK (wrong type, but still a table)
    local pem = crypto.convertJwkToPem({ kty = 'INVALID' })
    assert(pem == nil, "convertJwkToPem with invalid JWK should fail")

    -- Missing kty in JWK
    pem = crypto.convertJwkToPem({})
    assert(pem == nil, "convertJwkToPem with missing kty should fail")
end

local function test_negative_csr_generation()
    -- Invalid key
    local csr = crypto.generateCsr('notakey', 'CN=bad')
    assert(csr == nil, "generateCsr with invalid key should fail")
end

-- Add additional tests for edge cases in crypto functions

-- Test RSA key size variations
local function test_rsa_key_sizes()
    -- Test 2048-bit keys
    local priv_key_2048, pub_key_2048 = crypto.generateKeyPair("rsa", 2048)
    assert(type(priv_key_2048) == "string", "2048-bit private key type")
    assert(type(pub_key_2048) == "string", "2048-bit public key type")

    -- Test 4096-bit keys
    local priv_key_4096, pub_key_4096 = crypto.generateKeyPair("rsa", 4096)
    assert(type(priv_key_4096) == "string", "4096-bit private key type")
    assert(type(pub_key_4096) == "string", "4096-bit public key type")

    -- Test signing and verification with different key sizes
    local message = "Test message for RSA key sizes"

    local signature_2048 = crypto.sign("rsa", priv_key_2048, message, "sha256")
    assert(type(signature_2048) == "string", "2048-bit key signature")

    local is_valid_2048 = crypto.verify("rsa", pub_key_2048, message, signature_2048, "sha256")
    assert(is_valid_2048 == true, "2048-bit key verification")

    local signature_4096 = crypto.sign("rsa", priv_key_4096, message, "sha256")
    assert(type(signature_4096) == "string", "4096-bit key signature")

    local is_valid_4096 = crypto.verify("rsa", pub_key_4096, message, signature_4096, "sha256")
    assert(is_valid_4096 == true, "4096-bit key verification")
end

-- Test ECDSA curves
local function test_ecdsa_curves()
    local curves = { "secp256r1", "secp384r1", "secp521r1" }
    local message = "Test message for ECDSA curves"

    for _, curve in ipairs(curves) do
        local priv_key, pub_key = crypto.generateKeyPair("ecdsa", curve)
        assert(type(priv_key) == "string", curve .. " private key type")
        assert(type(pub_key) == "string", curve .. " public key type")

        local signature = crypto.sign("ecdsa", priv_key, message, "sha256")
        assert(type(signature) == "string", curve .. " signature")
        local is_valid = crypto.verify("ecdsa", pub_key, message, signature, "sha256")
        assert(is_valid == true, curve .. " verification")
    end
end

-- Test AES key sizes
local function test_aes_key_sizes()
    local key_sizes = { 128, 192, 256 }
    local plaintext = "Test message for AES key sizes"

    for _, size in ipairs(key_sizes) do
        local key = crypto.generateKeyPair("aes", size)
        assert(type(key) == "string", size .. "-bit AES key type")
        assert(#key == size / 8, size .. "-bit AES key length")

        -- Test CBC mode
        local ciphertext, iv = crypto.encrypt("aes", key, plaintext, { mode = "cbc" })
        assert(type(ciphertext) == "string", size .. "-bit AES CBC encryption")
        local decrypted_plaintext_cbc = crypto.decrypt("aes", key, ciphertext, { mode = "cbc", iv = iv })
        assert(decrypted_plaintext_cbc == plaintext, size .. "-bit AES CBC decryption")

        -- Test CTR mode
        local ciphertext_ctr, iv_ctr = crypto.encrypt("aes", key, plaintext, { mode = "ctr" })
        assert(type(ciphertext_ctr) == "string", size .. "-bit AES CTR encryption")
        local decrypted_plaintext_ctr = crypto.decrypt("aes", key, ciphertext_ctr, { mode = "ctr", iv = iv_ctr })
        assert(decrypted_plaintext_ctr == plaintext, size .. "-bit AES CTR decryption")

        -- Test GCM mode
        local ciphertext_gcm, iv_gcm, tag = crypto.encrypt("aes", key, plaintext, { mode = "gcm" })
        assert(type(ciphertext_gcm) == "string", size .. "-bit AES GCM encryption")
        local decrypted_plaintext_gcm = crypto.decrypt("aes", key, ciphertext_gcm, { mode = "gcm", iv = iv_gcm, tag = tag })
        assert(decrypted_plaintext_gcm == plaintext, size .. "-bit AES GCM decryption")
    end
end

-- Test AES decryption with corrupted ciphertext and tag
local function test_aes_corruption_handling()
    local key = crypto.generateKeyPair('aes', 256)
    local plaintext = "Sensitive data for corruption test"
    -- CBC mode
    local ciphertext, iv = crypto.encrypt("aes", key, plaintext, { mode = "cbc" })

    -- Corrupt ciphertext
    Log(kLogVerbose," - CBC decryption with corrupted ciphertext should fail")
    local corrupted = ciphertext:sub(1, #ciphertext - 1) .. string.char((ciphertext:byte(-1) ~ 0xFF) % 256)
    local plaintext_cbc = crypto.decrypt("aes", key, corrupted, { mode = "cbc", iv = iv })
    assert(plaintext_cbc == nil, "CBC decryption with corrupted ciphertext should fail")

    -- CTR mode (should not error, but output will be wrong)
    Log(kLogVerbose," - CTR decryption with corrupted ciphertext should not match original")
    local ciphertext_ctr, iv_ctr = crypto.encrypt("aes", key, plaintext, { mode = "ctr" })
    local corrupted_ctr = ciphertext_ctr:sub(1, #ciphertext_ctr - 1) ..
    string.char((ciphertext_ctr:byte(-1) ~ 0xFF) % 256)
    local plaintext_ctr = crypto.decrypt("aes", key, corrupted_ctr, { mode = "ctr", iv = iv_ctr })
    assert(plaintext_ctr ~= plaintext, "CTR decryption with corrupted ciphertext should not match original")

    -- GCM mode (should fail authentication)
    Log(kLogVerbose,"GCM decryption with corrupted ciphertext should fail")
    local ciphertext_gcm, iv_gcm, tag = crypto.encrypt("aes", key, plaintext, { mode = "gcm" })
    local corrupted_gcm = ciphertext_gcm:sub(1, #ciphertext_gcm - 1) ..
    string.char((ciphertext_gcm:byte(-1) ~ 0xFF) % 256)
    local plaintext_gcm = crypto.decrypt("aes", key, corrupted_gcm, { mode = "gcm", iv = iv_gcm, tag = tag })
    assert(plaintext_gcm == nil, "GCM decryption with corrupted ciphertext should fail")

    -- GCM mode with corrupted tag
    Log(kLogVerbose,"GCM decryption with corrupted tag should fail")
    local badtag = tag:sub(1, #tag - 1) .. string.char((tag:byte(-1) ~ 0xFF) % 256)
    local plaintext_gcm2 = crypto.decrypt("aes", key, ciphertext_gcm, { mode = "gcm", iv = iv_gcm, tag = badtag })
    assert(plaintext_gcm2 ~= plaintext, "GCM decryption with corrupted tag should fail")
end

-- Test AES encryption/decryption with empty plaintext
local function test_aes_empty_plaintext()
    local key = crypto.generateKeyPair('aes', 256)
    local empty = ""
    for _, mode in ipairs({ "cbc", "ctr", "gcm" }) do
        local ciphertext, iv, tag = crypto.encrypt("aes", key, empty, { mode = mode })
        assert(type(ciphertext) == "string", "AES " .. mode .. " encrypt empty string")

        local opts = { mode = mode, iv = iv, tag = tag }
        if mode ~= "gcm" then opts.tag = nil end

        local plaintext = crypto.decrypt("aes", key, ciphertext, opts)
        assert(plaintext == empty, "AES " .. mode .. " decrypt empty string")
    end
end

-- Test sign/verify with empty message
local function test_sign_verify_empty_message()
    local priv_key, pub_key = crypto.generateKeyPair("rsa", 2048)
    local signature = crypto.sign("rsa", priv_key, "", "sha256")
    assert(type(signature) == "string", "RSA sign empty message")

    local is_valid = crypto.verify("rsa", pub_key, "", signature, "sha256")
    assert(is_valid == true, "RSA verify empty message")

    local ec_priv, ec_pub = crypto.generateKeyPair("ecdsa", "secp256r1")
    local ec_sig = crypto.sign("ecdsa", ec_priv, "", "sha256")
    assert(type(ec_sig) == "string", "ECDSA sign empty message")

    local ec_valid = crypto.verify("ecdsa", ec_pub, "", ec_sig, "sha256")
    assert(ec_valid == true, "ECDSA verify empty message")
end

-- Test JWK to PEM with minimal valid JWKs and missing fields
local function test_jwk_to_pem_minimal()
    -- Minimal valid RSA public JWK
    local _, pub_key = crypto.generateKeyPair("rsa", 2048)
    local pub_jwk = crypto.convertPemToJwk(pub_key)
    local minimal_jwk = { kty = pub_jwk.kty, n = pub_jwk.n, e = pub_jwk.e }
    Log(kLogVerbose," - Testing minimal JWK to PEM conversion")
    local pem = crypto.convertJwkToPem(minimal_jwk)
    assert(type(pem) == "string", "Minimal RSA JWK to PEM")

    -- Missing 'n' field
    Log(kLogVerbose," - Testing missing 'n' field in JWK to PEM conversion")
    local bad_jwk = { kty = "RSA", e = pub_jwk.e }
    local pem2 = crypto.convertJwkToPem(bad_jwk)
    assert(pem2 == nil, "JWK to PEM with missing n should fail")

    -- Minimal EC public JWK
    Log(kLogVerbose," - Testing minimal EC JWK to PEM conversion")
    local _, ec_pub = crypto.generateKeyPair("ecdsa", "secp256r1")
    local ec_jwk = crypto.convertPemToJwk(ec_pub)
    local minimal_ec_jwk = { kty = ec_jwk.kty, crv = ec_jwk.crv, x = ec_jwk.x, y = ec_jwk.y }
    local ec_pem = crypto.convertJwkToPem(minimal_ec_jwk)
    assert(type(ec_pem) == "string", "Minimal EC JWK to PEM")

    -- Missing 'x' field
    Log(kLogVerbose," - Testing missing 'x' field in EC JWK to PEM conversion")
    local bad_ec_jwk = { kty = "EC", crv = ec_jwk.crv, y = ec_jwk.y }
    local ec_pem2 = crypto.convertJwkToPem(bad_ec_jwk)
    assert(ec_pem2 == nil, "EC JWK to PEM with missing x should fail")
end

-- Test PEM to JWK with corrupted PEM
local function test_pem_to_jwk_corrupted()
    local badpem = "-----BEGIN PUBLIC KEY-----\nMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA7\n-----END PUBLIC KEY-----"
    local ok = pcall(function() return crypto.convertPemToJwk(badpem) end)
    assert(ok == false, "PEM to JWK with corrupted PEM should fail")
end

-- Test CSR generation with missing/invalid subject/SAN
local function test_csr_generation_edge_cases()
    local priv_key, _ = crypto.generateKeyPair()
    -- Missing subject and SAN
    local csr = crypto.generateCsr(priv_key)
    assert(csr == nil, "CSR with missing subject and SAN should fail")
    -- Invalid SAN type (not validated yet)
    -- local csr2, err2 = crypto.generateCsr(priv_key, "CN=foo", 12345)
    -- assert(csr2 == nil, "CSR with invalid SAN type should fail")
end

-- Test unsupported AES mode
local function test_unsupported_aes_mode()
    Log(kLogVerbose," - AES decrypt with unsupported mode should fail")
    local key = crypto.generateKeyPair('aes', 256)
    local ciphertext = crypto.encrypt('aes', key, 'data', { mode = 'ofb' })
    assert(ciphertext == nil, "AES encrypt with unsupported mode should fail")

    local plaintext = crypto.decrypt('aes', key, 'data', { mode = 'ofb', iv = string.rep('A', 16) })
    assert(plaintext == nil, "AES decrypt with unsupported mode should fail")
end

-- Test encrypting and signing very large messages
local function test_large_message_handling()
    local priv_key, pub_key = crypto.generateKeyPair("rsa", 2048)
    local large_message = string.rep("A", 1024 * 1024) -- 1MB

    -- RSA encryption (should fail or be limited by key size)
    Log(kLogVerbose," - RSA encrypt large message should fail or be limited")
    local ciphertext = crypto.encrypt("rsa", pub_key, large_message)
    assert(ciphertext == nil, "RSA encrypt large message should fail or be limited")

    -- AES encryption (should succeed)
    Log(kLogVerbose," - AES encrypt large message")
    local key = crypto.generateKeyPair('aes', 256)
    local aes_ciphertext, iv = crypto.encrypt("aes", key, large_message, { mode = "cbc" })
    assert(type(aes_ciphertext) == "string", "AES encrypt large message")

    local decrypted_large_message = crypto.decrypt("aes", key, aes_ciphertext, { mode = "cbc", iv = iv })
    assert(decrypted_large_message == large_message, "AES decrypt large message")

    -- RSA sign large message
    Log(kLogVerbose," - RSA verify large message")
    local signature = crypto.sign("rsa", priv_key, large_message, "sha256")
    assert(type(signature) == "string", "RSA sign large message")

    local is_valid = crypto.verify("rsa", pub_key, large_message, signature, "sha256")
    assert(is_valid == true, "RSA verify large message")
end

-- Test passing non-string values as keys/messages/options
local function test_invalid_types()
    local priv_key, _ = crypto.generateKeyPair("rsa", 2048)
    local key = crypto.generateKeyPair('aes', 256)

    -- Non-string message
    Log(kLogVerbose," - RSA sign with integer message should fail")
    local signature = crypto.sign("rsa", priv_key, 12345, "sha256")
    assert(signature == nil, "RSA sign with non-string message should fail")

    Log(kLogVerbose," - AES encrypt with boolean message should fail")
    ciphertext = crypto.encrypt("aes", key, true, { mode = "cbc" })
    assert(ciphertext == nil, "AES encrypt with boolean message should fail")

    -- Non-string key
    Log(kLogVerbose," - RSA sign with table as key should fail")
    signature = crypto.sign("rsa", {}, "msg", "sha256")
    assert(signature == nil, "RSA sign with table as key should fail")

    -- Non-table options
    Log(kLogVerbose," - AES encrypt with number as options should fail")
    ciphertext = crypto.encrypt("aes", key, "msg", 123)
    assert(ciphertext == nil, "AES encrypt with number as options should fail")
end

-- Test encrypting with one mode and decrypting with another
local function test_mixed_mode_operations()
    local key = crypto.generateKeyPair('aes', 256)
    local plaintext = "Mixed mode test"
    local ciphertext, iv = crypto.encrypt("aes", key, plaintext, { mode = "cbc" })
    local decrypted_plaintext = crypto.decrypt("aes", key, ciphertext, { mode = "ctr", iv = iv })
    assert(decrypted_plaintext ~= plaintext, "Decrypt CBC ciphertext with CTR mode should not match")

    local ciphertext2, iv2 = crypto.encrypt("aes", key, plaintext, { mode = "ctr" })
    local decrypted_plaintext2 = crypto.decrypt("aes", key, ciphertext2, { mode = "cbc", iv = iv2 })
    assert(decrypted_plaintext2 ~= plaintext, "Decrypt CTR ciphertext with CBC mode should not match")
end

-- Test signing/verifying/converting with nil or empty parameters
local function test_nil_empty_parameters()
    Log(kLogVerbose," - RSA sign with nil message should fail")
    local priv_key, pub_key = crypto.generateKeyPair("rsa", 2048)
    local signature = crypto.sign("rsa", priv_key, nil, "sha256")
    assert(signature == nil, "Sign with nil message should fail")

    Log(kLogVerbose," - RSA verify with nil message should fail")
    local is_valid = crypto.verify("rsa", pub_key, nil, "sig", "sha256")
    assert(is_valid == nil, "Verify with nil message should fail")

    Log(kLogVerbose," - JWK to PEM with nil should fail")
    local ok = pcall(function() return crypto.convertJwkToPem(nil) end)
    assert(ok == false, "convertJwkToPem with nil should fail")

    Log(kLogVerbose," - JWK to PEM with empty string should fail")
    ok = pcall(function() return crypto.convertJwkToPem("") end)
    assert(ok == false, "convertJwkToPem with empty string should fail")
end

-- Run all tests
local function run_tests()
    Log(kLogVerbose,"Testing RSA keypair generation...")
    test_rsa_keypair_generation()
    Log(kLogVerbose,"Testing RSA key size variations...")
    test_rsa_key_sizes()
    Log(kLogVerbose,"Testing RSA signing and verification...")
    test_rsa_signing_verification()
    Log(kLogVerbose,"Testing RSA encryption and decryption...")
    test_rsa_encryption_decryption()
    Log(kLogVerbose,"Testing RSA-PSS signing and verification...")
    test_rsapss_signing_verification()

    Log(kLogVerbose,"Testing ECDSA keypair generation...")
    test_ecdsa_keypair_generation()
    Log(kLogVerbose,"Testing ECDSA signing and verification...")
    test_ecdsa_signing_verification()
    Log(kLogVerbose,"Testing ECDSA curves...")
    test_ecdsa_curves()

    Log(kLogVerbose,"Testing AES key generation...")
    test_aes_key_generation()
    Log(kLogVerbose,"Testing AES encryption and decryption (CBC mode)...")
    test_aes_encryption_decryption_cbc()
    Log(kLogVerbose,"Testing AES encryption and decryption (CTR mode)...")
    test_aes_encryption_decryption_ctr()
    Log(kLogVerbose,"Testing AES encryption and decryption (GCM mode)...")
    test_aes_encryption_decryption_gcm()
    Log(kLogVerbose,"Testing unsupported AES mode...")
    test_unsupported_aes_mode()
    Log(kLogVerbose,"Testing AES key sizes...")
    test_aes_key_sizes()
    Log(kLogVerbose,"Testing AES decryption with corrupted ciphertext and tag...")
    test_aes_corruption_handling()
    Log(kLogVerbose,"Testing AES encryption/decryption with empty plaintext...")
    test_aes_empty_plaintext()
    Log(kLogVerbose,"Testing large message encryption and signing...")
    test_large_message_handling()

    Log(kLogVerbose,"Testing various hash algorithms...")
    test_hash_algorithms()
    Log(kLogVerbose,"Testing negative cases for hash algorithms...")
    test_negative_hash_algorithms()
    Log(kLogVerbose,"Testing sign/verify with empty message...")
    test_sign_verify_empty_message()
    Log(kLogVerbose,"Testing invalid input types...")
    test_invalid_types()
    Log(kLogVerbose,"Testing mixed mode encryption/decryption...")
    test_mixed_mode_operations()
    Log(kLogVerbose,"Testing nil/empty parameters...")
    test_nil_empty_parameters()
    Log(kLogVerbose,"Testing negative keypair generation...")
    test_negative_keypair_generation()
    Log(kLogVerbose,"Testing negative encrypt/decrypt...")
    test_negative_encrypt_decrypt()
    Log(kLogVerbose,"Testing negative sign/verify...")
    test_negative_sign_verify()

    Log(kLogVerbose,"Testing PEM to JWK conversion...")
    test_pem_to_jwk()
    Log(kLogVerbose,"Testing PEM to JWK with corrupted PEM...")
    test_pem_to_jwk_corrupted()
    Log(kLogVerbose,"Testing JWK to PEM conversion...")
    test_jwk_to_pem()
    Log(kLogVerbose,"Testing negative PEM/JWK conversion...")
    test_negative_pem_jwk_conversion()
    Log(kLogVerbose,"Testing JWK to PEM with minimal valid JWKs and missing fields...")
    test_jwk_to_pem_minimal()
    Log(kLogVerbose,"Testing CSR generation...")
    test_csr_generation()
    Log(kLogVerbose,"Testing CSR generation with missing/invalid subject/SAN...")
    test_csr_generation_edge_cases()
    Log(kLogVerbose,"Testing negative CSR generation...")
    test_negative_csr_generation()
    EXIT = 0
    return EXIT
end

EXIT = 70

os.exit(run_tests())
