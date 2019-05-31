CTAGS = -M

# CAST_OBJ := multicast.o cJSON.o common.o
OBJ := md5.o test.o
# TCP_S_OBJ := tcp_server.o cJSON.o common.o dispatchcmd.o

# OBJ := hope_halo.o multicast.o cJSON.o common.o tcp_server.o dispatchcmd.o
LINK_LIB := -lm -lpthread
.PHONY: clean
# all : multicast multitest tcp_server
all: test

test:$(OBJ)
	gcc -o test $(OBJ) $(LINK_LIB)

# multicast:$(CAST_OBJ)
# 	gcc -o multicast $(CAST_OBJ) $(LINK_LIB)
# tcp_server:$(TCP_S_OBJ)
# 	gcc -o tcp_server $(TCP_S_OBJ) $(LINK_LIB)
# common.o:
# 	gcc -c common.c	

# cJSON.o:
# 	gcc -c cJSON.c	-lm

clean:
	-rm *.o test
