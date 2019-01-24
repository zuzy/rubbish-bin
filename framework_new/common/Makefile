OBJ = common.o
.PHONY: clean


# serial:$(OBJ)
# 	gcc -c $(CFLAGS) $(OBJ) $(LINK_LIB)
# 	gcc -o serial $(OBJ) $(LINK_LIB)

all:common
common:$(OBJ)
	gcc -c $(OBJ) -lm -lpthread
	gcc -o common $(OBJ) -lpthread
clean:
	-rm common *.o