OBJ:=test_getopt.o

.PHONY:all clean

all:test_getopt test_getopt_long

test_getopt:$(OBJ)
	gcc -o test_getopt $(OBJ)
test_getopt_long:test_getopt_long.o
	gcc -o test_getopt_long test_getopt_long.o

clean:
	-rm -f *.o test_getopt test_getopt_long