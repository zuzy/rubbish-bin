TARGET := $(notdir $(PWD))

# SRC := $(wildcard *.c)
SRC := $(wildcard ./src/*.c)
INC := $(wildcard ./inc/*.h)
OBJ := $(patsubst %.c,%.o, $(wildcard *.c))
CC := gcc
SHARED_LIB_PATH:= 
SHARED_LIB_CFLAGS:= -Wl,-rpath=.

GLOBAL_CFLAGS := -fPIC -g

EXTR_CFLAGS:= -Dlinux

INCLUDE_LOCAL_DIRS:=-I../inc 

LINK_LIB := -lm -lpthread -lmicrohttpd

C_FLAGS := $(EXTR_CFLAGS) $(GLOBAL_CFLAGS) 

TAR_PATH := $(shell pwd)
TMP_PATH := __tmp__/
INSTALL_PATH := ./

.PHONY: clean test fresh install

all:
	@-mkdir $(TMP_PATH)
	@cp $(SRC) $(INC) Makefile $(TMP_PATH)
	@echo $(TARGET)
	@make -C $(TMP_PATH) $(TARGET)
	@-cp $(TMP_PATH)$(TARGET) $(TAR_PATH)

$(TARGET): $(OBJ)
	@$(CC) -o $@ $^ $(LINK_LIB) $(SHARED_LIB_PATH) $(SHARED_LIB_CFLAGS)

%.o: %.c
	@$(CC) -c $(C_FLAGS) $< -o $@ $(INCLUDE_LOCAL_DIRS)

install:
	@cp -rf $(TARGET) $(INSTALL_PATH)

fresh: clean all

test:
	./$(TARGET)

clean:
	@-rm $(TMP_PATH) $(TARGET) -rf
	@# -rm $(TARGET)
	@# -rm *.o $(TARGET)

