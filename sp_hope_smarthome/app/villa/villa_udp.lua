#!/usr/bin/lua

function villa_scan( sn )
    local json = require("cjson")
    local socket = require("socket.core")
    local udp = socket.udp()
    udp:setoption('broadcast', true)
    local host = '0.0.0.0'
    local port = '7777'
    local bro_host = '255.255.255.255'
    local bro_port = '6666'
    udp:settimeout(1)
    udp:setsockname(host, port)
    local scan_tab = {}
    scan_tab.type = 'REQUEST_TCP'
    scan_tab.sn = sn
    -- local find_str = '{"type":"REQUEST_TCP","sn":"12345678"}'
    local find_str = json.encode(scan_tab)
    print(find_str)
    while 1 do
        local sendcli = udp:sendto(find_str,bro_host,bro_port)
        if(sendcli) then
            print('sendcli ok')
            local revbuff,receip,receport = udp:receivefrom()
            if (revbuff and receip and receport) then
                return true, json.decode(revbuff)
            else
                print('waiting client connect')
            end
        else
            print('sendcli error')
        end
        socket.sleep(3)
    end
    udp:close()
    
end

-- local ret, tab = villa_scan('123457678234532')
-- print("villa ret", ret)
-- for i, v in ipairs(tab) do
--     print(i, v)
-- end
-- print(tab.type, tab.sn)