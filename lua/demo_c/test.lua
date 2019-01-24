#!/usr/bin/lua
function test3( ... )
    print("----- test myCustomLib")
    -- package.cpath = "./?.so" --so搜寻路劲
    package.cpath = "/usr/lib/?.so" --so搜寻路劲
    -- local f = require "myLualib" -- 对应luaopen_myLualib中的myLualib
    -- local f = require ("myLualib")
    local f = require ("libhope_lua_api")
 
    f.test1(123)
    f.test2("hello world")
    f.test3(456, "yangx", 'bye')

end
 
test3()
