NAME = halotest
OUTPUT:=../out/usr/bin

EXTR_CFLAGS :=

LINK_STATIC_LIB:=
LINK_SHARED_LIB:=  -lpthread -lapplib -lrt -lapconfig \
                   -ldl -lconfigpart -lwifi_client \
				   -lm \
				   -lalooper \
				   -lstream_source \
				   -ljson-c \
				   -lmlang \
				   -lmedia_navi_api \
				   -lcares \
				   -lmedia_navi_api \
				   -lmusic_player_api \
				   -lsmart_player_api \
                   -L../../out/usr/lib 
				#    -ltp_long_double_api

INCLUDE_LOCAL_DIRS:=  -I../include \
                      -I. \
					  -Iapi -I../../../ucgui/src/gui/include/gui \
                      -I../../../libsrc/src/include \

#SRCDIR    = $(shell find -type d)
SRCDIR    =.
# SRC_C  = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c)) #zizy
# SRC_S  = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S)) #zizy
include ../app.rules
