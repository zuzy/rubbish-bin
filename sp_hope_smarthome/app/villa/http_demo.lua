#!/usr/bin/lua
local http = require("socket.http")

local body
local str, err, h = http.request{
    url = 'http://www.bing.com',
    method = 'GET',
    sink = body,}
print(str, err, body)

for key, v in pairs(h) do
    print (key, v)
end

print('---------------------------------')

-- r, c, h = http.request("http://www.bing.com")
-- print(r, c)

-- for key, v in pairs(h) do
--     print (key, v)
-- end