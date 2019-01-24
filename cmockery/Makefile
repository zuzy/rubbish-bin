# INC=-I /usr/local/include/google
# LIB=-L /usr/local/lib
# SRC=math.c test_math.c
SRC=common.c test_math.c math.c
TARGET=math_test_run
LINK_LIB := -lm -lpthread -Wall -lcmockery
all: $(SRC)
	# gcc $^ -o $(TARGET) $(INC) $(LIB) -lcmockery -lpthread -lm
	gcc $^ -o $(TARGET) $(LINK_LIB)
clean:
	-rm *.o $(TARGET)

.PHONY:clean