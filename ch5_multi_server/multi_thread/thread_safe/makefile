
# server: server.o
# 	cc -o server server.o -lpthread
# server.o: server.c
##	cc -c server.c


# objects := $(wildcard *.o)

# VPAHT = ../simple
# objects := $(patsubst %.c,%.o,$(wildcard *.c))
# server: $(objects)
# 	cc -o server $(objects) -lpthread

objects := client.o server.o
all : server client

server: server.o
	gcc -o server server.o -lpthread
client: client.o
	gcc -o client client.o -lpthread

# $(objects) : %.o : %.c
# 	gcc -c $< -o $@ -lpthread



.PHONY: cleanall print clean
print: *.c
	lpr -p $?
	touch print
cleanall: clean	
	@echo cleaning
	@-rm server client
clean: 
	@# cat *.c
	@-rm *.o