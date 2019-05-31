objects := main.o
all : main

main: $(objects)
	# gcc -Wall -o main $^ -L/usr/local/lib/lua/5.3/cjson.so -I/usr/local/lib/lua/5.3 -llua -lm -ldl
	gcc -o main $(objects) -I/usr/include/lua5.3 -llua -lm -ldl

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
