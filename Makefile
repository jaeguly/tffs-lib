#
# make file for tffs
# 
# knightray@gmail.com
# 10/27 2008
#

#
# build target definiation.
#
LIB_TARGET=libtffs.a
TEST_TARGET=tffs
TSH_TARGET=tsh

#
# source file definiation
#
LIBSRC=src/hai_file.c src/initfs.c src/debug.c src/fat.c src/dir.c src/dirent.c src/common.c src/file.c src/crtdef.c src/cache.c
TESTSRC=src/main.c
TSHSRC=tshell/tsh.c

#
# object files definiation
#
LIBOBJ=$(LIBSRC:.c=.o)
TESTOBJ=$(TESTSRC:.c=.o)
TSHOBJ=$(TSHSRC:.c=.o)

#
# makefile parameters
#
CC=gcc
CFLAGS=-Wall -g -I ./inc
LFLAGS=

all:$(LIB_TARGET)
test:$(TEST_TARGET)

$(LIB_TARGET):$(LIBOBJ)
	ar rcs $(LIB_TARGET) $(LIBOBJ)
	#rm -fr src/*.o

$(TEST_TARGET):$(TESTOBJ)
	$(CC) $(LFLAGS) -o $(TEST_TARGET) $(TESTOBJ) -L./ -ltffs

$(TSH_TARGET):$(TSHOBJ)
	$(CC) $(LFLAGS) -o $(TSH_TARGET) $(TSHOBJ) -L./ -ltffs

clean:
	rm -f src/*.o
	rm -f tshell/*.o
	rm -f $(TEST_TARGET)
	rm -f $(LIB_TARGET)
	rm -f $(TSH_TARGET)

