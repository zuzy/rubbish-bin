NAME := libch2num
OUTPUT:=../../../out/usr/lib/lua

EXTR_CFLAGS := -Wall -fPIC -W -Waggregate-return -Wcast-align -Wmissing-prototypes -Wnested-externs -Wshadow -Wwrite-strings -pedantic
LINK_STATIC_LIB:=
LINK_SHARED_LIB:=  -llua -lm

INCLUDE_LOCAL_DIRS:= -I../../include \
					-I../api \
					-I.

#SRCDIR    = $(shell find -type d)
SRCDIR    =.
SRC_C  = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_S  = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))

include ../../../shared_lib.rules

# .PHONY: release_1
# all : release_1
# release_1 :
# 	echo "lib chinese num trans"
# 	mkdir -p ../../../out/usr/lib/lua
# 	cp -f ./num2ch.lua ../../../out/usr/lib/lua