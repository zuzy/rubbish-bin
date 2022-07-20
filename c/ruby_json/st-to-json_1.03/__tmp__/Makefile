TARGET := demo

# SRC := $(wildcard *.c)
SRC := $(wildcard ./src/*.c)
INC := $(wildcard ./inc/*.h)
OBJ := $(patsubst %.c,%.o, $(wildcard *.c))

SHARED_LIB_PATH:=
SHARED_LIB_CFLAGS:= -Wl,-rpath=$(SHARED_LIB_PATH)

GLOBAL_CFLAGS := -fPIC $(SHARED_LIB_CFLAGS) -ftime-report -std=c++17 -Ofast -DNDEBUG

EXTR_CFLAGS:= -Dlinux

INCLUDE_LOCAL_DIRS:=-I../inc

LINK_LIB := -lm -lpthread

C_FLAGS := $(INCLUDE_LOCAL_DIRS) $(EXTR_CFLAGS) $(GLOBAL_CFLAGS) $(LINK_LIB) 

TAR_PATH := $(shell pwd)
TMP_PATH := __tmp__/

.PHONY: clean test fresh

all:
	@-mkdir $(TMP_PATH)
	@cp $(SRC) $(INC) Makefile $(TMP_PATH)
	@make -C $(TMP_PATH) main
	@-cp $(TMP_PATH)$(TARGET) $(TAR_PATH)

main:$(TARGET)
	@# echo $(OBJ) $(TARGET)
	@gcc $(OBJ) -o $(TARGET) $(LINK_LIB) $(SHARED_LIB_CFLAGS)

$(TARGET): $(OBJ)
	@# echo $(OBJ) "target"
	@# gcc -c $(C_FLAGS) $< -o $@

%.o: %.c
	@gcc -c $(C_FLAGS) $< -o $@
fresh: clean all

test:
	./$(TARGET)

clean:
	@-rm $(TMP_PATH) $(TARGET) -rf
	@# -rm $(TARGET)
	@# -rm *.o $(TARGET)

