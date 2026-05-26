What is it?
------

This directory contains fuzz targets.
Fuzz targets are simple codes using the library.
They are used with a so-called fuzz driver, which will generate inputs, try to process them with the fuzz target, and alert in case of an unwanted behavior (such as a buffer overflow for instance).

These targets were meant to be used with oss-fuzz but can be used in other contexts.

This code was contributed by Philippe Antoine ( Catena cyber ).

How to run?
------

To run the fuzz targets like oss-fuzz:
```
git clone https://github.com/google/oss-fuzz
cd oss-fuzz
python infra/helper.py build_image mbedtls
python infra/helper.py build_fuzzers --sanitizer address mbedtls
python infra/helper.py run_fuzzer mbedtls fuzz_client
```
You can use `undefined` sanitizer as well as `address` sanitizer.
And you can run any of the fuzz targets like `fuzz_client`.

To run the fuzz targets without oss-fuzz, you first need to install one libFuzzingEngine (libFuzzer for instance).
Then you need to compile the code with the compiler flags of the wished sanitizer.
```
perl scripts/config.py set MBEDTLS_PLATFORM_TIME_ALT
mkdir build
cd build
cmake ..
make
```
Finally, you can run the targets like `./test/fuzz/fuzz_client`.


Corpus generation for network traffic targets
------

These targets use network traffic as inputs :
* client : simulates a client against (fuzzed) server traffic
* server : simulates a server against (fuzzed) client traffic
* dtls_client
* dtls_server

They also use the last bytes as configuration options.

To generate corpus for these targets, you can do the following, not fully automated steps :
* Build mbedtls programs ssl_server2 and ssl_client2
* Run them one against the other with `reproducible` option turned on while capturing traffic into test.pcap
* Extract tcp payloads, for instance with tshark : `tshark -Tfields -e tcp.dstport -e tcp.payload -r test.pcap > test.txt`
* Run a dummy python script to output either client or server corpus file like `python dummy.py test.txt > test.cor`
* Finally, you can add the options by appending the last bytes to the file test.cor

Here is an example of dummy.py for extracting payload from client to server (if we used `tcp.dstport` in tshark command)
```
import sys
import binascii

f = open(sys.argv[1])
for l in f.readlines():
    portAndPl=l.split()
    if len(portAndPl) == 2:
        # determine client or server based on port
        if portAndPl[0] == "4433":
            print(binascii.unhexlify(portAndPl[1].replace(":","")))
```
