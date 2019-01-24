NAME = libsair_ai

EXTR_CFLAGS:=

LINK_STATIC_LIB:=
LINK_SHARED_LIB:=  -lpthread -lapplib -lrt \
                   -lm -L../../../out/usr/lib 

LINK_SHARED_LIB+=  -L./

INCLUDE_LOCAL_DIRS:=  -I../../../include \
                      -I../../include \
                      -I.

SRCDIR    =.
OUTPUT =../../../out/usr/lib

include ../../../shared_lib.rules

.PHONY: release_1
all : release_1
release_1 :
	#cp -af ./*.so* ../../../out/usr/lib

