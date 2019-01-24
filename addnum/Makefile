objects:= addnum.o
target:= addnum
cc:=gcc

# $(objects):
all: $(objects)
	$(cc) -o $(target) $(objects) -lpthread
	@echo "make success"

.PHONY: clean
clean: 
	@-rm *.o $(target)