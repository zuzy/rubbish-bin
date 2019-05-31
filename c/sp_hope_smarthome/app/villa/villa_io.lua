#!/usr/bin/lua

local json = require("cjson")
local para_file = '/tmp/villa_param.lua'

function write_init( str )
    local f = io.open(para_file, "w+")
    f:write(str)
end

function write_json_file( str )
    local f = io.open(para_file, "a+")
    f:write(str)
    io.close(f)
end

