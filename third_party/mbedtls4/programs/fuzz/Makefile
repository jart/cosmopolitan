MBEDTLS_TEST_PATH:=../../tests

MBEDTLS_PATH := ../..
include ../../scripts/common.make

PROGRAM_FUZZ_PATH:=$(MBEDTLS_PATH)/tf-psa-crypto/programs/fuzz

DEP=${MBEDLIBS}

ifdef FUZZINGENGINE
LOCAL_LDFLAGS += -lFuzzingEngine
endif

LOCAL_CFLAGS += -I$(PROGRAM_FUZZ_PATH)

# A test application is built for each fuzz_*.c file.
APPS = $(basename $(wildcard fuzz_*.c))
APPS += $(basename $(PROGRAM_FUZZ_PATH)/fuzz_privkey.c)
APPS += $(basename $(PROGRAM_FUZZ_PATH)/fuzz_pubkey.c)

# Construct executable name by adding OS specific suffix $(EXEXT).
BINARIES := $(addsuffix $(EXEXT),$(APPS))

.SILENT:

.PHONY: all check test clean

all: $(BINARIES)

C_FILES := $(addsuffix .c,$(APPS))

%.o: %.c
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -c $<	-o $@


ifdef FUZZINGENGINE
$(BINARIES): %$(EXEXT): %.o $(PROGRAM_FUZZ_PATH)/fuzz_common.o $(DEP)
	echo " $(CC) $(PROGRAM_FUZZ_PATH)/fuzz_common.o $< $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@"
	$(CXX) $(PROGRAM_FUZZ_PATH)/fuzz_common.o $<	$(LOCAL_LDFLAGS) $(LDFLAGS) -o $@
else
$(BINARIES): %$(EXEXT): %.o $(PROGRAM_FUZZ_PATH)/fuzz_common.o $(PROGRAM_FUZZ_PATH)/fuzz_onefile.o $(DEP)
	echo " $(CC) $(PROGRAM_FUZZ_PATH)/fuzz_common.o $(PROGRAM_FUZZ_PATH)/fuzz_onefile.o $< $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@"
	$(CC) $(PROGRAM_FUZZ_PATH)/fuzz_common.o $(PROGRAM_FUZZ_PATH)/fuzz_onefile.o $<	$(LOCAL_LDFLAGS) $(LDFLAGS) -o $@
endif

clean:
ifndef WINDOWS
	rm -rf $(BINARIES) *.o
	rm -rf $(MBEDTLS_PATH)/tf-psa-crypto/programs/fuzz/*.o
else
	if exist *.o del /Q /F *.o
	if exist *.exe del /Q /F *.exe
	if exist $(MBEDTLS_PATH)\tf-psa-crypto\programs\fuzz\*.o del /Q /F $(MBEDTLS_PATH)\tf-psa-crypto\programs\fuzz\*.o
endif
