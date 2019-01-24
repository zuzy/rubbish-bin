
# server: server.o
# 	cc -o server server.o -lpthread
# server.o: server.c
##	cc -c server.c


# objects := $(wildcard *.o)
VPATH = ../simple
objects := $(patsubst %.c,%.o,$(wildcard *.c))
server: $(objects)
	cc -o server $(objects) -lpthread


.PHONY: clean print
print: *.c
	lpr -p $?
	touch print
clean: 
	# cat *.c
	-rm *.o server