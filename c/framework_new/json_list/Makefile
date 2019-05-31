OBJ = 	cJSON.o \
		cjson_list.o

LINK_LIB := -lm -lpthread -Wall
WARN := -Wall
.PHONY: clean


# serial:$(OBJ)
# 	gcc -c $(CFLAGS) $(OBJ) $(LINK_LIB)
# 	gcc -o serial $(OBJ) $(LINK_LIB)

all:cjson_list
cjson_list:$(OBJ)
	@# gcc $(WARN) $(OBJ) $(LINK_LIB)
	gcc -o cjson_list $(OBJ) $(LINK_LIB)
	-rm *.o
clean:
	-rm common *.o