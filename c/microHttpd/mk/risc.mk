TARGET := $(notdir $(PWD))

# SRC := $(wildcard *.c)
SRC := $(wildcard ./src/*.c)
INC := $(wildcard ./inc/*.h)
OBJ := $(patsubst %.c,%.o, $(wildcard *.c))
# CC := gcc
CC := /home/zizy/project/ykx/yikexin-ys15-toplevel/ys15/share/gcc-yikexin-8.3.0-x86_64_riscv64-linux-gnu/bin/riscv64-yikexin-linux-gnu-gcc

SHARED_LIB_PATH:= -L/home/zizy/project/ykx/nb1-repo/freedom-u-sdk/work/buildroot_initramfs_sysroot/usr/lib
# SHARED_LIB_CFLAGS:= -Wl,-rpath=$(SHARED_LIB_PATH)
SHARED_LIB_CFLAGS:= -Wl,-rpath=.

GLOBAL_CFLAGS := -fPIC

# EXTR_CFLAGS:= -Dlinux

INCLUDE_LOCAL_DIRS:=-I../inc \
			-I/home/zizy/project/ykx/nb1-repo/freedom-u-sdk/work/buildroot_initramfs/host/riscv64-buildroot-linux-gnu/sysroot/usr/include/

LINK_LIB := -lm -lpthread -lmicrohttpd
# LINK_LIB := -lm -lpthread

C_FLAGS := $(EXTR_CFLAGS) $(GLOBAL_CFLAGS) 

TAR_PATH := $(shell pwd)
TMP_PATH := __tmp__/

.PHONY: clean test fresh

all:
	@-mkdir $(TMP_PATH)
	@cp $(SRC) $(INC) Makefile $(TMP_PATH)
	@echo $(TARGET)
	@make -C $(TMP_PATH) $(TARGET)
	@-cp $(TMP_PATH)$(TARGET) $(TAR_PATH)

$(TARGET): $(OBJ)
	@echo $(OBJ) "target" $@ $<
	@$(CC) -o $@ $< $(LINK_LIB) $(SHARED_LIB_PATH) $(SHARED_LIB_CFLAGS)

%.o: %.c
	$(CC) -c $(C_FLAGS) $< -o $@ $(INCLUDE_LOCAL_DIRS)

fresh: clean all

test:
	./$(TARGET)

clean:
	@-rm $(TMP_PATH) $(TARGET) -rf
	@# -rm $(TARGET)
	@# -rm *.o $(TARGET)

