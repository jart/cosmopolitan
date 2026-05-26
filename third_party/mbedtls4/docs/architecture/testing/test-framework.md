# Mbed TLS test framework

This document is an overview of the Mbed TLS test framework and test tools.

This document is incomplete. You can help by expanding it.

## Unit tests

See <https://mbed-tls.readthedocs.io/en/latest/kb/development/test_suites>

### Unit test descriptions

Each test case has a description which succinctly describes for a human audience what the test does. The first non-comment line of each paragraph in a `.data` file is the test description. The following rules and guidelines apply:

* Test descriptions may not contain semicolons, line breaks and other control characters, or non-ASCII characters. <br>
  Rationale: keep the tools that process test descriptions (`generate_test_code.py`, [outcome file](#outcome-file) tools) simple.
* Test descriptions must be unique within a `.data` file. If you can't think of a better description, the convention is to append `#1`, `#2`, etc. <br>
  Rationale: make it easy to relate a failure log to the test data. Avoid confusion between cases in the [outcome file](#outcome-file).
* Test descriptions should be a maximum of **66 characters**. <br>
  Rationale: 66 characters is what our various tools assume (leaving room for 14 more characters on an 80-column line). Longer descriptions may be truncated or may break a visual alignment. <br>
  We have a lot of test cases with longer descriptions, but they should be avoided. At least please make sure that the first 66 characters describe the test uniquely.
* Make the description descriptive. “foo: x=2, y=4” is more descriptive than “foo #2”. “foo: 0<x<y, both even” is even better if these inequalities and parities are why this particular test data was chosen.
* Avoid changing the description of an existing test case without a good reason. This breaks the tracking of failures across CI runs, since this tracking is based on the descriptions.

`framework/scripts/check_test_cases.py` enforces some rules and warns if some guidelines are violated.

## TLS tests

### SSL extension tests

#### SSL test case descriptions

Each test case in `ssl-opt.sh` has a description which succinctly describes for a human audience what the test does. The test description is the first parameter to `run_test`.

The same rules and guidelines apply as for [unit test descriptions](#unit-test-descriptions). In addition, the description must be written on the same line as `run_test`, in double quotes, for the sake of `check_test_cases.py`.

### SSL cipher suite tests

Each test case in `compat.sh` has a description which succinctly describes for a human audience what the test does. The test description is `$TITLE` defined in `run_client`.

The same rules and guidelines apply as for [unit test descriptions](#unit-test-descriptions). In addition, failure cause in `compat.sh` is not classified as `ssl-opt.sh`, so the information of failed log files are followed as prompt.

## Running tests

### Outcome file

#### Generating an outcome file

Unit tests, `ssl-opt.sh` and `compat.sh` record the outcome of each test case in a **test outcome file**. This feature is enabled if the environment variable `MBEDTLS_TEST_OUTCOME_FILE` is set. Set it to the path of the desired file.

If you run `all.sh --outcome-file test-outcome.csv`, this collects the outcome of all the test cases in `test-outcome.csv`.

#### Outcome file format

The outcome file is in a CSV format using `;` (semicolon) as the delimiter and no quoting. This means that fields may not contain newlines or semicolons. There is no title line.

The outcome file has 6 fields:

* **Platform**: a description of the platform, e.g. `Linux-x86_64` or `Linux-x86_64-gcc7-msan`.
* **Configuration**: a unique description of the configuration (`mbedtls_config.h`).
* **Test suite**: `test_suite_xxx`, `ssl-opt` or `compat`.
* **Test case**: the description of the test case.
* **Result**: one of `PASS`, `SKIP` or `FAIL`.
* **Cause**: more information explaining the result.
