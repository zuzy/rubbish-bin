LUA_LIB:=../../../out/usr/lib/lua
OUTPUT:=../../../out/usr/bin
all:
	mkdir -p $(LUA_LIB)
	mkdir -p $(OUTPUT)
	-cp -rf ./*.lua $(LUA_LIB)
	-cp -rf ./*.lua $(OUTPUT)
clean:
	rm -rf $(LUA_LIB)
	rm -rf $(OUTPUT)/*.lua
.PHONY:  clean