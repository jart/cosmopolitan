@rem Generate automatically-generated configuration-independent source files
@rem and build scripts.
@rem Requirements:
@rem * Perl must be on the PATH ("perl" command).
@rem * Python 3.8 or above must be on the PATH ("python" command).
@rem * Either a C compiler called "cc" must be on the PATH, or
@rem   the "CC" environment variable must point to a C compiler.

@rem @@@@ tf-psa-crypto @@@@
cd tf-psa-crypto
python framework\scripts\make_generated_files.py || exit /b 1
cd ..

@rem @@@@ mbedtls @@@@
python scripts\make_generated_files.py || exit /b 1
