


OBJ :=  cJSON.o common.o halo_udp.o halo_tcp_srv.o halo_tcp_cli.o halo_uart.o main.o
LINK_LIB := -lm -lpthread

.PHONY: clean


# serial:$(OBJ)
# 	gcc -c $(CFLAGS) $(OBJ) $(LINK_LIB)
# 	gcc -o serial $(OBJ) $(LINK_LIB)

all:common
cJSON.o:
	gcc -c cJSON.c	-lm
common:$(OBJ)
	gcc -c $(OBJ) $(LINK_LIB)
	gcc -o common $(OBJ) $(LINK_LIB)
clean:
	-rm common *.o