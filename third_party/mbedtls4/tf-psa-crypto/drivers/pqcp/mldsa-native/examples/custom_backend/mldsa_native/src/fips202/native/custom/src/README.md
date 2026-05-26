[//]: # (SPDX-License-Identifier: MIT)

# tiny_sha3
Very small, readable implementation of the FIPS 202 and SHA3 hash function.
Public domain.

### Updated 27-Dec-15:

Added SHAKE128 and SHAKE256 code and test vectors. The code can actually do
a XOF of arbitrary size (like "SHAKE512").


### Updated 03-Sep-15:

Made the implementation portable. The API is now pretty much the
same that OpenSSL uses.


### Updated 07-Aug-15:

Now that SHA3 spec is out, I've updated the package to match with the
new padding rules. There is literally one line difference between
Keccak 3.0 and SHA-3 implementations:

```
    temp[inlen++] = 0x06;           // XXX Padding Changed from Keccak 3.0
```

The 0x06 constant there used to be 0x01. But this of course totally
breaks compatibility and test vectors had to be revised.

SHA-3 Spec: http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf

Cheers,
- markku


### Original README.TXT from 19-Nov-11:

Hi.

The SHA-3 competition is nearing it's end and I would personally like
to support Keccak as the winner. I have a PhD in hash function cryptanalysis
so don't take my word for it, go ahead and look into the code !

Since I couldn't find a *compact* and/or *readable* implementation of Keccak
anywhere, here's one I cooked up as a service to the curious.

This implementation is intended for study of the algorithm, not for
production use.

The code works correctly on 64-bit little-endian platforms with gcc.
Like your Linux box. The main.c module contains self-tests for all
officially supported hash sizes.

If you're looking for production code, the official multi-megabyte package
covers everything you could possibly need and too much much more:
http://keccak.noekeon.org/

Cheers,
- Markku  19-Nov-11

Dr. Markku-Juhani O. Saarinen <mjos@iki.fi>
