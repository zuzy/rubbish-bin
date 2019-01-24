CFLAGS += -MMD
-include ...#*.d


OBJ := demo_serial.o
OBJ_CLIENT:=client.o
OBJ_SERVER:=server.o
LINK_LIB := -lm -lpthread
.PHONY: clean
# all : multicast multitest tcp_server
all: client server

# serial:$(OBJ)
# 	gcc -c $(CFLAGS) $(OBJ) $(LINK_LIB)
# 	gcc -o serial $(OBJ) $(LINK_LIB)

# multicast:$(CAST_OBJ)
# 	gcc -o multicast $(CAST_OBJ) $(LINK_LIB)
# multitest:$(TEST_OBJ)
# 	gcc -o multitest $(TEST_OBJ) $(LINK_LIB)
server:$(OBJ_SERVER)
	gcc -o server $(OBJ_SERVER) $(LINK_LIB)
client:$(OBJ_CLIENT)
	gcc -o client $(OBJ_CLIENT) $(LINK_LIB)
	# common.o:
# 	gcc -c common.c	

# cJSON.o:
# 	gcc -c cJSON.c	-lm

clean:
	-rm *.o server client *.d

