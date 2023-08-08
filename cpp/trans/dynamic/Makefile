all: test
test: 
	g++ -g -fprofile-arcs -ftest-coverage -O3 -g0 -DNDEBUG -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -m64 -march=x86-64 -mtune=generic -D TOPSRIDER_FALLTHROUGH= -Werror=array-bounds -Werror=empty-body -Werror=format-extra-args -Werror=bool-compare -Werror=address -Werror=free-nonheap-object -Werror=overflow -Werror=return-local-addr -Werror=unused-value -Werror=switch-bool -Werror=switch -Werror=write-strings -Werror=unused-function -Werror=conversion-null -Werror=delete-incomplete -Werror=delete-non-virtual-dtor -Werror=shift-count-overflow -Werror=sizeof-pointer-memaccess -Werror=unused-label -Werror=unreachable-code -Werror=format -Werror=format-security -Werror=varargs -Wdouble-promotion -Wno-error=narrowing - -fno-canonical-system-headers -Wno-narrowing -Wno-attributes -Wno-reorder -Wno-sign-compare -Wno-double-promotion -Wno-unused-function -fno-inline -Werror -Wall -Wextra -Werror=sequence-point -Werror=comment -Werror=parentheses -Werror=type-limits -Werror=ignored-qualifiers -Werror=unused-variable -Werror=uninitialized -Werror=enum-compare -Werror=unknown-pragmas -Werror=pointer-arith -Werror=trigraphs -Werror=char-subscripts -Werror=shift-count-negative -Werror=logical-not-parentheses -Werror=return-type -Werror=maybe-uninitialized -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-strict-aliasing -Wno-maybe-uninitialized -Wno-type-limits -Wno-unknown-pragmas -std=gnu++14 -fstack-protector -ffunction-sections -fdata-sections -fno-omit-frame-pointer -DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_MATH_DISABLE_FLOAT128 -DTENSORFLOW_USE_CUSTOM_CONTRACTION_KERNEL -DTENSORFLOW_USE_MKLDNN_CONTRACTION_KERNEL -DTF_USE_SNAPPY -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -D__DISABLE_LLVM_EXPORT__ -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=1 -std=gnu++14 dynamic.cc -O test

TARGET := $(notdir $(PWD))

# SRC := $(wildcard *.c)
SRC := $(wildcard ./*.cc)
INC := $(wildcard ./*.h)
OBJ := $(patsubst %.c,%.o, $(wildcard *.c))
CC := g++
# CC := /home/zizy/project/ykx/yikexin-ys15-toplevel/ys15/share/gcc-yikexin-8.3.0-x86_64_riscv64-linux-gnu/bin/riscv64-yikexin-linux-gnu-gcc

SHARED_LIB_PATH:= -L/home/zizy/project/ykx/nb1-repo/freedom-u-sdk/work/buildroot_initramfs_sysroot/usr/lib
SHARED_LIB_CFLAGS:= -Wl,-rpath=.

GLOBAL_CFLAGS := -g -fprofile-arcs -ftest-coverage -O3 -g0 -DNDEBUG -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -m64 -march=x86-64 -mtune=generic -D TOPSRIDER_FALLTHROUGH= -Werror=array-bounds -Werror=empty-body -Werror=format-extra-args -Werror=bool-compare -Werror=address -Werror=free-nonheap-object -Werror=overflow -Werror=return-local-addr -Werror=unused-value -Werror=switch-bool -Werror=switch -Werror=write-strings -Werror=unused-function -Werror=conversion-null -Werror=delete-incomplete -Werror=delete-non-virtual-dtor -Werror=shift-count-overflow -Werror=sizeof-pointer-memaccess -Werror=unused-label -Werror=unreachable-code -Werror=format -Werror=format-security -Werror=varargs -Wdouble-promotion -Wno-error=narrowing - -fno-canonical-system-headers -Wno-narrowing -Wno-attributes -Wno-reorder -Wno-sign-compare -Wno-double-promotion -Wno-unused-function -fno-inline -Werror -Wall -Wextra -Werror=sequence-point -Werror=comment -Werror=parentheses -Werror=type-limits -Werror=ignored-qualifiers -Werror=unused-variable -Werror=uninitialized -Werror=enum-compare -Werror=unknown-pragmas -Werror=pointer-arith -Werror=trigraphs -Werror=char-subscripts -Werror=shift-count-negative -Werror=logical-not-parentheses -Werror=return-type -Werror=maybe-uninitialized -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-strict-aliasing -Wno-maybe-uninitialized -Wno-type-limits -Wno-unknown-pragmas -std=gnu++14 -fstack-protector -ffunction-sections -fdata-sections -fno-omit-frame-pointer -DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_MATH_DISABLE_FLOAT128 -DTENSORFLOW_USE_CUSTOM_CONTRACTION_KERNEL -DTENSORFLOW_USE_MKLDNN_CONTRACTION_KERNEL -DTF_USE_SNAPPY -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -D__DISABLE_LLVM_EXPORT__ -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=1 -std=gnu++14

EXTR_CFLAGS:= 

# INCLUDE_LOCAL_DIRS:=-I../inc \
# 			-I/home/zizy/project/ykx/nb1-repo/freedom-u-sdk/work/buildroot_initramfs/host/riscv64-buildroot-linux-gnu/sysroot/usr/include/

# LINK_LIB := -lm -lpthread -lmicrohttpd

C_FLAGS := $(EXTR_CFLAGS) $(GLOBAL_CFLAGS) 

TAR_PATH := $(shell pwd)
TMP_PATH := __tmp__/

INSTALL_PATH := ~/nfs

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

fresh: clean all

install:
	@cp -rf $(TARGET) $(INSTALL_PATH)

test:
	./$(TARGET)

clean:
	@-rm $(TMP_PATH) $(TARGET) -rf
	@# -rm $(TARGET)
	@# -rm *.o $(TARGET)

