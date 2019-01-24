OBJ = common.o \
		main_loop.o \
		main_pipe.o \
		tcp_client.o \
		udp_ser.o \
		tcp_ser.o \
		tcp_ser_cli.o \
		cJSON.o \
		tcp_ser_dispatch.o \
		cjson_list.o \
		tcp_cli_dispatch.o \
		villa.o \
		villa_dispatch.o

LINK_LIB := -lm -lpthread -Wall
WARN := -Wall
.PHONY: clean


# serial:$(OBJ)
# 	gcc -c $(CFLAGS) $(OBJ) $(LINK_LIB)
# 	gcc -o serial $(OBJ) $(LINK_LIB)

all:common
common:$(OBJ)
	@# gcc $(WARN) $(OBJ) $(LINK_LIB)
	gcc -o common $(OBJ) $(LINK_LIB)
	-rm *.o
clean:
	-rm common *.o