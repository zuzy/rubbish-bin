-include ../sair.conf

.PHNOY:all clean
all:
ifeq ($(AI_SUPPORT_NULL), y)
	make -C ./null
else ifeq ($(AI_SUPPORT_HOBOT), y)
	make -C ./hobot
else ifeq ($(AI_SUPPORT_ROOBO), y)
	make -C ./roobo	
else ifeq ($(AI_SUPPORT_TULING), y)
	make -C ./tuling
else ifeq ($(AI_SUPPORT_SPEECH), y)
	make -C ./speech
endif


clean:
ifeq ($(AI_SUPPORT_NULL), y)
	make -C ./null clean
else ifeq ($(AI_SUPPORT_HOBOT), y)
	make -C ./hobot clean
else ifeq ($(AI_SUPPORT_ROOBO), y)
	make -C ./roobo	clean
else ifeq ($(AI_SUPPORT_SPEECH), y)
	make -C ./speech   clean
endif


