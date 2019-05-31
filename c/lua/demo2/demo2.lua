#!/usr/bin/lua

-- local s = '打开'
-- print(s:find('开'))
-- local helo = {
--     aaa = 1,
--     bbb = 2,
-- }

-- local tab = {
--     helo = "world"
-- }


-- if helo['aaa'] then
--     print(tab.helo)
-- end
-- print(tostring(tab))

local json = require("cjson")

local zone = -1
function dump_tab( tab )
    zone = zone + 1
    for k, v in pairs(tab) do
        local str = "\t"
        str = str:rep(zone)
        if type(v) == 'table' then
            print(str..k)
            dump_tab(v)
        else
            print(str..k, v)
        end
    end
    zone = zone - 1
end

local tab = {}
local tt = {}
tt.name = 1
tt.id = 2
local data = {}
data[1] = tt
tab.data = data
-- dump_tab(tab)
tab.name = 1111
print(json.encode(tab))
tab.name = nil
print(json.encode(tab))
-- for k, v in pairs(tab.data) do
--     print(k, v)
-- end

