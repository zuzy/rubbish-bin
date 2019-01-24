objects:= pub_list.o mqttclient.o buffer.o buff_parse.o parse_pipe.o
all: mqttclient
mqttclient:$(objects)
	gcc -o mqttclient $(objects) -lpthread


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