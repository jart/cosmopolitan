ifeq ($(wildcard ../../tf-psa-crypto),)
TF_PSA_CRYPTO_PATH ?= ../..
MBEDTLS_PATH ?= ../../..
else
MBEDTLS_PATH ?= ../..
TF_PSA_CRYPTO_PATH ?= ../../tf-psa-crypto
endif

include $(TF_PSA_CRYPTO_PATH)/scripts/crypto-common.make

CFLAGS += -Wall -Werror -std=c99 -D_XOPEN_SOURCE=1 -D_POSIX_C_SOURCE=200809L

ifeq ($(DEBUG),1)
override CFLAGS += -DDEBUG -O0 -g
endif

CLIENT_LIBS := -Lclient_libs -lpsaclient -lmbedtls -lmbedx509 -lmbedcrypto
SERVER_LIBS := -Lserver_libs -lmbedcrypto

COMMON_INCLUDE := -I./include -I$(MBEDTLS_PATH)/include \
	$(TF_PSA_CRYPTO_LIBRARY_PUBLIC_INCLUDE)

GENERATED_MANIFEST_H_FILES = \
	include/psa_manifest/manifest.h \
	include/psa_manifest/pid.h \
	include/psa_manifest/sid.h

GENERATED_WRAPPER_H_FILES = \
	src/psa_sim_serialise.h \
	src/psa_functions_codes.h

GENERATED_H_FILES = $(GENERATED_MANIFEST_H_FILES) $(GENERATED_WRAPPER_H_FILES)

GENERATED_C_FILES = \
	src/psa_sim_serialise.c \
	src/psa_sim_crypto_client.c src/psa_sim_crypto_server.c

PSASIM_GENERATED_FILES = $(GENERATED_H_FILES) $(PARTITION_SERVER_BOOTSTRAP) $(GENERATED_C_FILES)

LIBPSACLIENT_SRC = src/psa_ff_client.c \
		 		src/psa_sim_crypto_client.c \
		 		src/psa_sim_serialise.c
LIBPSACLIENT_OBJS=$(LIBPSACLIENT_SRC:.c=.o)

PSA_CLIENT_BASE_SRC = $(LIBPSACLIENT_SRC) src/client.c

PSA_CLIENT_FULL_SRC = $(LIBPSACLIENT_SRC) \
				$(wildcard src/aut_*.c)

PARTITION_SERVER_BOOTSTRAP = src/psa_ff_bootstrap_TEST_PARTITION.c

PSA_SERVER_SRC = $(PARTITION_SERVER_BOOTSTRAP) \
				 src/psa_ff_server.c \
				 src/psa_sim_crypto_server.c \
				 src/psa_sim_serialise.c

.PHONY: all clean client_libs server_libs

all:

generated_files: $(PSASIM_GENERATED_FILES)

test/seedfile:
	dd if=/dev/urandom of=./test/seedfile bs=64 count=1

src/%.o: src/%.c $(GENERATED_H_FILES)
	$(CC) $(COMMON_INCLUDE) $(CFLAGS) -c $< $(LDFLAGS) -o $@

client_libs/libpsaclient: $(LIBPSACLIENT_OBJS)
	mkdir -p client_libs
	$(AR) -src client_libs/libpsaclient.a $(LIBPSACLIENT_OBJS)

test/psa_client_base: $(PSA_CLIENT_BASE_SRC) $(GENERATED_H_FILES) test/seedfile
	$(CC) $(COMMON_INCLUDE) $(CFLAGS) $(PSA_CLIENT_BASE_SRC) $(CLIENT_LIBS) $(LDFLAGS) -o $@

test/psa_client_full: $(PSA_CLIENT_FULL_SRC) $(GENERATED_H_FILES) test/seedfile
	$(CC) $(COMMON_INCLUDE) $(CFLAGS) $(PSA_CLIENT_FULL_SRC) $(CLIENT_LIBS) $(LDFLAGS) -o $@

test/psa_server: $(PSA_SERVER_SRC) $(GENERATED_H_FILES)
	$(CC) $(COMMON_INCLUDE) $(CFLAGS) $(PSA_SERVER_SRC) $(SERVER_LIBS) $(LDFLAGS) -o $@

$(PARTITION_SERVER_BOOTSTRAP) $(GENERATED_MANIFEST_H_FILES): src/manifest.json src/server.c
	tools/psa_autogen.py src/manifest.json

src/psa_sim_serialise.h: src/psa_sim_serialise.pl
	cd src && ./psa_sim_serialise.pl h >$(@F)

src/psa_sim_serialise.c: src/psa_sim_serialise.pl
	cd src && ./psa_sim_serialise.pl c >$(@F)

src/psa_functions_codes.h src/psa_sim_crypto_client.c src/psa_sim_crypto_server.c: $(TF_PSA_CRYPTO_PATH)/include/psa/crypto.h
src/psa_functions_codes.h src/psa_sim_crypto_client.c src/psa_sim_crypto_server.c: $(TF_PSA_CRYPTO_PATH)/include/psa/crypto_extra.h
src/psa_functions_codes.h src/psa_sim_crypto_client.c src/psa_sim_crypto_server.c: src/psa_sim_generate.pl
	cd $(TF_PSA_CRYPTO_PATH) && $(abspath src/psa_sim_generate.pl)

# Build MbedTLS libraries (crypto, x509 and tls) and copy them locally to
# build client/server applications.
#
# Note: these rules assume that mbedtls_config.h is already configured by all.sh.
# If not using all.sh then the user must do it manually.
client_libs: client_libs/libpsaclient
client_libs server_libs:
	$(MAKE) -C $(MBEDTLS_PATH)/library CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" libmbedcrypto.a libmbedx509.a libmbedtls.a
	mkdir -p $@
	cp $(MBEDTLS_PATH)/library/libmbed*.a $@/

clean_server_intermediate_files:
	rm -f $(PARTITION_SERVER_BOOTSTRAP)
	rm -rf include/psa_manifest

clean: clean_server_intermediate_files
	rm -f test/psa_client_base test/psa_client_full test/psa_server
	rm -rf client_libs server_libs
	rm -f test/psa_service_* test/psa_notify_* test/*.log
	rm -f test/seedfile

neat: clean
	rm -f $(PSASIM_GENERATED_FILES)
