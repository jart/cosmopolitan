# change this variable to point to the directory with Lua headers
# of the version being tested
LUA_DIR = ../../

CC = gcc

# compilation should generate Dynamic-Link Libraries
CFLAGS = -Wall -std=gnu99 -O2 -I$(LUA_DIR) -fPIC -shared

# libraries used by the tests
all: lib1.so lib11.so lib2.so lib21.so lib2-v2.so
	touch all

lib1.so: lib1.c $(LUA_DIR)/luaconf.h
	$(CC) $(CFLAGS) -o lib1.so lib1.c

lib11.so: lib11.c $(LUA_DIR)/luaconf.h
	$(CC) $(CFLAGS) -o lib11.so lib11.c

lib2.so: lib2.c $(LUA_DIR)/luaconf.h
	$(CC) $(CFLAGS) -o lib2.so lib2.c

lib21.so: lib21.c $(LUA_DIR)/luaconf.h
	$(CC) $(CFLAGS) -o lib21.so lib21.c

lib2-v2.so: lib21.c $(LUA_DIR)/luaconf.h
	$(CC) $(CFLAGS) -o lib2-v2.so lib22.c
