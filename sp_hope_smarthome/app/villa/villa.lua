#!/usr/bin/lua
require("villa_common")
require("villa_udp")
require("villa_tcp")
require("socket")
local sn = 'hi-2017-04-26'

local cbs = {
    villa_info_device,
    villa_info_scene,
    villa_info_room,
    villa_info_floor,
    villa_info_attribute,
}

local ret, scan = villa_scan(sn)
if not ret then 
    print("scan error") 
else
    print(ret, scan.type, scan.data.ip, scan.data.port, scan.data.company)
    -- villa_client(scan.data.ip, scan.data.port, 3)
    local co = coroutine.create( villa_client )
    local ret, status, sock = coroutine.resume(co, scan.data.ip, scan.data.port, 3)
    print(ret, status)
    for i, v in ipairs(cbs) do
        v(sock)
        ret, status, sock = coroutine.resume( co )
        socket.sleep(1)
    end
    -- villa_info_device(sock, sn)
    local time = socket.gettime();
    print("clock",time);
    if true then
        while true do
            ret, status, sock = coroutine.resume( co )
            print(ret, status)
            print("clock",socket.gettime());
            if socket.gettime() - time > 5 then
                time = socket.gettime()
                villa_heart(sock)
            end
        end
    end
end



function receive (prod)
    local status, value = coroutine.resume(prod)
    return value
end

function send (x)
    coroutine.yield(x)
end

