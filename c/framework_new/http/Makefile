OBJ = halo_utf8.o http.o
.PHONY: clean


# serial:$(OBJ)
# 	gcc -c $(CFLAGS) $(OBJ) $(LINK_LIB)
# 	gcc -o serial $(OBJ) $(LINK_LIB)

all:http
http:$(OBJ)
	gcc -c $(OBJ) -lm -lpthread
	gcc -o http $(OBJ) -lpthread
clean:
	-rm http *.o