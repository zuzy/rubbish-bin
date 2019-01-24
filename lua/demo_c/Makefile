CC ?= gcc
CFLAGS = -g -O2 -Wall -I$(LUA_INC)
SHARED := -fPIC --shared
 
TARGET = myLualib.so
LUA_CLIB_PATH = clib
 
# 引入lua头文件
 
# LUA_INC ?= /root/lua-5.3.0/src
LUA_INC ?= /usr/local/lib/
 
start: $(TARGET)
 
$(TARGET) : myLualib.c | $(LUA_CLIB_PATH)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@
	@-cp -rf *.so $(LUA_CLIB_PATH)
 
clean:
	rm -fr $(TARGET)
 
$(LUA_CLIB_PATH) :
	mkdir $(LUA_CLIB_PATH)
