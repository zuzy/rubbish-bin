#!/usr/bin/lua
local wifi = hpio.wifi
local player = hpio.player

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

function get_wifi_status( str )
    print(str)
    local tab = wifi.info()
    dump_tab(tab)
    tab = player.status()
    dump_tab(tab)
    return 1
end
