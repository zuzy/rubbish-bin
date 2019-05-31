#!/usr/bin/lua
package.path = '/tmp/lua/?.lua'
require("villa_common")
local md5 = require("md5")
local http = require("socket.http")

function hope_tab2str( tab )
    local str = ""
    if type(tab) ~= 'table' then return nil end

    for key, val in pairs(tab) do
        str = str..tostring(key)..'='..val..'&'
    end
    return string.sub( str, 1, -2 )
end

function checkout(tab)
    local str = ""
    if tab.len and tab.key and tab.cid and tab.sid and tab.ver then
        str = tab.len..tab.key..tab.cid..tab.sid..tab.ver
    end
    local m_str = md5.sumhexa(str)
    m_str = string.upper( m_str )
    local check = 0
    for i = 1, #m_str do
        check = bitoper(check, string.byte(m_str, i), XOR)
    end
    return string.format("%02X", check)
end

local tab = {
    ver = '1.0',
    -- des = '79',
    cid = '750064224428658688',
    sid = '750837261197414400',
    key = 'A716A953593940D2BD78E1A02CD3C070',
    -- len = '62',
    -- dat = '{"deviceGuid":"00:00:6C:06:A6:29","deviceSN":"70368170428025"}',
}

function http_post( uri, dat )
    tab.dat = dat
    tab.len = utf8_len(dat)
    tab.des = checkout(tab)
    print('http post '..dat)
    dump_tab(tab)
    local str, err = http.request(uri, hope_tab2str(tab))
    return str, err
end

