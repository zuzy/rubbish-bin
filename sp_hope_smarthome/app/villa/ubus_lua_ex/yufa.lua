#!/usr/bin/lua
local json = require("cjson")

local file = '/tmp/yufa.txt'

local str = io.input(file):read("*a")

local tab = {}

tab.name = str

local out_file = '/tmp/yufa_out.txt'

-- local out = json.encode(tab)

-- local out = str:gsub('"', '\"')

local out = string.gsub( str,"\"","\\\"")

io.output(out_file):write(out)