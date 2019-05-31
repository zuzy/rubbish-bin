#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>



#include <dlfcn.h>

typedef void Register(lua_State*);

int luaopen_drmaa(lua_State *L){

    // void* lib = dlopen("/usr/local/lib/lua/5.3/cjson.so", RTLD_NOW | RTLD_GLOBAL);
    // if(!lib)
    // {
    //     printf("%s\n", dlerror());
    //     return 1;
    // }

    // Register* loadFunc = (Register*)dlsym(lib, "luaopen_test");
    // if(!loadFunc)
    // {
    //     printf("%s\n", dlerror());
    //     return 1;
    // }

    // loadFunc(L);

    return 0;
}


static void lua_stachDump(lua_State *L);

#if 0
int main(void)
{
    
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaL_dofile(L, "./test1.lua");
    lua_close(L);
    return 0;
}
#else
// #include <stdio.h>
// #include <string.h>
 
// #include <lua5.1/lua.h>
// #include <lua5.1/lualib.h>
// #include <lua5.1/lauxlib.h>
 
/* The lua interpreter */
lua_State *L;

int getfield(lua_State *L, char *key)
{
    int re;
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if(!lua_isnumber(L, -1))
        luaL_error(L, "not number");
    re = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);
    return re;
}

int luaadd(int x, int y)
{
    int sum;
 
    /* the function name */
    lua_getglobal(L, "des");

    // lua_getglobal(L, "json");
    int t = lua_type(L, -1);
    printf("type is %s\n", lua_typename(L, t));
    // int err = lua_error(L);
    // printf("err is %d", err);
    // lua_stachDump(L);
    /* the first argument */
    lua_pushnumber(L, x);
    // lua_stachDump(L);
    /* the second argument */
    lua_pushnumber(L, y);
    lua_stachDump(L);
    /* call the function with 2 arguments, return 1 result. */
    lua_call(L, 2, 1);
    lua_stachDump(L);
    /* get the result */
    sum = (int)lua_tonumber(L, -1);
    // lua_stachDump(L);
    /* cleanup the return */
    lua_pop(L, 1);
 
    lua_getglobal(L, "background");
    if(!lua_istable(L, -1))
        luaL_error(L, "background is not a table");
    
    int r = getfield(L, "r");
    int g = getfield(L, "g");
    int b = getfield(L, "b");
    printf("get r:%d g:%d b:%d\n\n\n", r,g,b);

    return sum;
}

static void lua_stachDump(lua_State *L)
{
    int i;
    int top = lua_gettop(L);
    printf("top : %d\n", top);
    for(i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        switch(t) {
            case LUA_TSTRING: {
                printf("%s", lua_tostring(L, i));
                break;
            }
            case LUA_TBOOLEAN: {
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
            }
            case LUA_TNUMBER: {
                printf("%g", lua_tonumber(L, i));
                break;
            }
            default: {
                printf("%s", lua_typename(L, t));
                break;
            }
        }
        printf("\n");
    }
    printf("[end]\n");
}

void test()
{
    lua_State *S = luaL_newstate();
    lua_pushboolean(S, 1);
    lua_pushnumber(S, 10);
    lua_pushnil(S);
    lua_pushstring(S, "hello");
    lua_stachDump(S);
    lua_pushvalue(S, 1);
    lua_stachDump(S);

    lua_close(S);
}

int lua_parse(char *des, char *key)
{
    int val; 
    /* the function name */
    lua_getglobal(L, "parse");

    int t = lua_type(L, -1);
    printf("type is %s\n", lua_typename(L, t));

    // lua_getglobal(L, "ssssss");
    // t = lua_type(L, -1);
    // printf("type is %s\n", lua_typename(L, t));

    lua_pushstring(L, des);
    lua_pushstring(L, key);
    /* call the function with 2 arguments, return 1 result. */
    lua_call(L, 2, 1);
    val = (int)lua_tonumber(L, -1);

    lua_pop(L, 1);

    return val;
}

char json_str[] = "{\"age\":\"23\",\"testArray\":{\"array\":[8,9,11,14,25]},\"Himi\":\"himigame.com\"}";
 
int main (int argc, char **argv)
{
    int sum;
    /* initialize lua */
    L = luaL_newstate();
        // luaL_loadfile(L, "cjson");
 
    /* load lua base libraries */

    luaL_openlibs(L);

    luaopen_drmaa(L);
 
    /* load the script */
    luaL_dofile(L, "test1.lua");
    lua_stachDump(L);
    /* call the add function */
    sum = luaadd(10, 15);
    // printf("The sum is %d \n", sum);
    // int age = lua_parse(json_str, "age");
    // printf("age is %d\n", age);
    /* print the result */
 
    /* cleanup lua */
    lua_close(L);
    // test();
    return 0;
} /* -----End of main()----- */


#endif