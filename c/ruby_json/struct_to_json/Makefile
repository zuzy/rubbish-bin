CTAGS = -M

# CAST_OBJ := multicast.o cJSON.o common.o
# TEST_OBJ := cJSON.o common.o
# TCP_S_OBJ := tcp_server.o cJSON.o common.o dispatchcmd.o

OBJ := cJSON.o cJSON_format.o main.o
LINK_LIB := -lm -lpthread
.PHONY: clean test
# all : multicast multitest tcp_server
all: demo

test: all
	./demo

demo:$(OBJ)
	gcc -o demo $(OBJ) $(LINK_LIB) -g

# multicast:$(CAST_OBJ)
# 	gcc -o multicast $(CAST_OBJ) $(LINK_LIB)
# multitest:$(TEST_OBJ)
# 	gcc -o multitest $(TEST_OBJ) $(LINK_LIB)
# tcp_server:$(TCP_S_OBJ)
# 	gcc -o tcp_server $(TCP_S_OBJ) $(LINK_LIB)
# common.o:
# 	gcc -c common.c	

cJSON.o:
	gcc -c cJSON.c	-lm

clean:
	-rm *.o demo

