CFLAGS += -MMD
-include ...#*.d


OBJ := demo_serial.o
LINK_LIB := -lm -lpthread
.PHONY: clean
# all : multicast multitest tcp_server
all: serial

serial:$(OBJ)
	gcc -c $(CFLAGS) $(OBJ) $(LINK_LIB)
	gcc -o serial $(OBJ) $(LINK_LIB)

# multicast:$(CAST_OBJ)
# 	gcc -o multicast $(CAST_OBJ) $(LINK_LIB)
# multitest:$(TEST_OBJ)
# 	gcc -o multitest $(TEST_OBJ) $(LINK_LIB)
# tcp_server:$(TCP_S_OBJ)
# 	gcc -o tcp_server $(TCP_S_OBJ) $(LINK_LIB)
# common.o:
# 	gcc -c common.c	

# cJSON.o:
# 	gcc -c cJSON.c	-lm

clean:
	-rm *.o serial *.d

