NAME := libhope_api
OUTPUT:=../../out/usr/lib

EXTR_CFLAGS := 
LINK_STATIC_LIB:=
LINK_SHARED_LIB:=  -lapplib

INCLUDE_LOCAL_DIRS:= -I../../include 

#SRCDIR    = $(shell find -type d)
SRCDIR    =.
SRC_C  = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_S  = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))

include ../../shared_lib.rules

