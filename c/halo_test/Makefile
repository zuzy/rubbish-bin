
# server: server.o
# 	cc -o server server.o -lpthread
# server.o: server.c
##	cc -c server.c


# objects := $(wildcard *.o)

# VPAHT = ../simple
# objects := $(patsubst %.c,%.o,$(wildcard *.c))
# server: $(objects)
# 	cc -o server $(objects) -lpthread

# objects := client.o server.o recv.o
# all : server client recv

# server: server.o
# 	gcc -o server server.o -lpthread
# client: client.o
# 	gcc -o client client.o -lpthread
# recv: recv.o 
# 	gcc -o recv recv.o 	

objects:= multi_server.o client.o main_task.o
all: multi client main_task
multi:multi.o
	gcc -o multi multi.o -lpthread
client:client.o	
	gcc -o client client.o -lpthread
main_task:main_task.o	
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
#	@# cat *.c
	@-rm *.o