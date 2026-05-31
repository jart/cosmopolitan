## Reporting Vulnerabilities

If you think you have found an Mbed TLS security vulnerability, then please
send an email to the security team at
<mbed-tls-security@lists.trustedfirmware.org>.

## Security Incident Handling Process

Our security process is detailed in our
[security
center](https://developer.trustedfirmware.org/w/mbed-tls/security-center/).

Its primary goal is to ensure fixes are ready to be deployed when the issue
goes public.

## Maintained branches

Only the maintained branches, as listed in [`BRANCHES.md`](BRANCHES.md),
get security fixes.
Users are urged to always use the latest version of a maintained branch.

## Use of TF-PSA-Crypto

Note that Mbed TLS uses the cryptography API provided by TF-PSA-Crypto.
Its
[threat model](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/SECURITY.md#threat-model)
applies to all cryptographic operations performed by Mbed TLS. In particular,
users of Mbed TLS should note the considerations around
[block ciphers](https://github.com/Mbed-TLS/TF-PSA-Crypto/blob/development/SECURITY.md#block-ciphers)
since they apply to the block ciphers used in TLS.

## Threat model

We classify attacks based on the capabilities of the attacker.

### Remote attacks

In this section, we consider an attacker who can observe and modify data sent
over the network. This includes observing the content and timing of individual
packets, as well as suppressing or delaying legitimate messages, and injecting
messages.

Mbed TLS aims to fully protect against remote attacks and to enable the user
application in providing full protection against remote attacks. Said
protection is limited to providing security guarantees offered by the protocol
being implemented. (For example Mbed TLS alone won't guarantee that the
messages will arrive without delay, as the TLS protocol doesn't guarantee that
either.)

### Local attacks

In this section, we consider an attacker who can run software on the same
machine. The attacker has insufficient privileges to directly access Mbed TLS
assets such as memory and files.

#### Timing attacks

The attacker is able to observe the timing of instructions executed by Mbed TLS
by leveraging shared hardware that both Mbed TLS and the attacker have access
to. Typical attack vectors include cache timings, memory bus contention and
branch prediction.

Mbed TLS provides limited protection against timing attacks. The cost of
protecting against timing attacks widely varies depending on the granularity of
the measurements and the noise present. Therefore the protection in Mbed TLS is
limited. We are only aiming to provide protection against **publicly
documented attack techniques**.

As attacks keep improving, so does Mbed TLS's protection. Mbed TLS is moving
towards a model of fully timing-invariant code, but has not reached this point
yet.

**Remark:** Timing information can be observed over the network or through
physical side channels as well. Remote and physical timing attacks are covered
in the [Remote attacks](remote-attacks) and [Physical
attacks](physical-attacks) sections respectively.

#### Local non-timing side channels

The attacker code running on the platform has access to some sensor capable of
picking up information on the physical state of the hardware while Mbed TLS is
running. This could for example be an analogue-to-digital converter on the
platform that is located unfortunately enough to pick up the CPU noise.

Mbed TLS doesn't make any security guarantees against local non-timing-based
side channel attacks. If local non-timing attacks are present in a use case or
a user application's threat model, they need to be mitigated by the platform.

#### Local fault injection attacks

Software running on the same hardware can affect the physical state of the
device and introduce faults.

Mbed TLS doesn't make any security guarantees against local fault injection
attacks. If local fault injection attacks are present in a use case or a user
application's threat model, they need to be mitigated by the platform.

### Physical attacks

In this section, we consider an attacker who has access to physical information
about the hardware Mbed TLS is running on and/or can alter the physical state
of the hardware (e.g. power analysis, radio emissions or fault injection).

Mbed TLS doesn't make any security guarantees against physical attacks. If
physical attacks are present in a use case or a user application's threat
model, they need to be mitigated by physical countermeasures.

### Caveats

#### Out-of-scope countermeasures

Mbed TLS has evolved organically and a well defined threat model hasn't always
been present. Therefore, Mbed TLS might have countermeasures against attacks
outside the above defined threat model.

The presence of such countermeasures don't mean that Mbed TLS provides
protection against a class of attacks outside of the above described threat
model. Neither does it mean that the failure of such a countermeasure is
considered a vulnerability.

#### Formatting of X509 data

This section discusses limitations in how X.509 objects are processed. This
applies to certificates, certificate signing requests (CSRs) and certificate
revocation lists (CRLs).

Mbed TLS does not check that they are strictly compliant with X.509 and other
relevant standards. In the case of signed certificates and signed CRLs, the
signing party is assumed to have performed this validation (and the certificate
or CRL is trusted to be correctly formatted as long as the signature is
correct).  Similarly, CSRs are implicitly trusted by Mbed TLS to be
standards-compliant.

**Warning!** Mbed TLS must not be used to sign untrusted CSRs or CRLs unless
extra validation is performed separately to ensure that they are compliant to
the relevant specifications. This makes Mbed TLS on its own unsuitable for use
in a Certificate Authority (CA).

However, Mbed TLS aims to protect against memory corruption and other
undefined behavior when parsing certificates, CSRs and CRLs. If a CSR or signed
certificate causes undefined behavior when it is parsed by Mbed TLS, that
is considered a security vulnerability.
