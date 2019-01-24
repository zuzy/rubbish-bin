#!/usr/bin/lua
require("socket")
require("villa_common")
require("villa_tcp_parse")
local json = require("cjson")

-- function u16_to_str(u)
--     if not u then
--         print("input is nill")
--         return nill
--     end
--     local s = math.modf(u/256)
--     return string.char(s)..string.char(u%256)
-- end

function villa_send(sock, str, cmd)
    local tt = {0xaa, len, cmd, data, chk}
    tt.data = str
    tt.len = #tt.data
    tt.cmd = cmd
    local str = u16_to_str(tt.len)..u16_to_str(tt.cmd)..tt.data
    tt.chk = 0;
    for i = 1, #str do
        tt.chk = bitoper(tt.chk, string.byte(str,i), XOR)
        -- chk = chk ~ string.byte(str. i)
    end
    str = string.char(tt[1]) .. str .. string.char(tt.chk)
    sock:send(str)
    print(tt.data)
    -- hex_dump(str)
end

function villa_heart(sock)
    villa_send(sock,'{"type":"REQUEST_HEART_BEAT"}', 1)
end

function villa_info_device(sock)
    local t = {}
    t.type = 'REQUEST_DEVICE'
    local str = json.encode(t)
    -- villa_send(sock, ' { "type":"REQUEST_SCENE"}', 7)
    villa_send(sock, str, 5)
end

function villa_info_scene(sock)
    local t = {}
    t.type = 'REQUEST_SCENE'
    local str = json.encode(t)
    villa_send(sock, str, 7)
end

function villa_info_room(sock)
    local t = {}
    t.type = 'REQUEST_ROOM'
    local str = json.encode(t)
    villa_send(sock, str, 9)
end

function villa_info_floor(sock)
    local t = {}
    t.type = 'REQUEST_FLOOR'
    local str = json.encode(t)
    -- villa_send(sock, ' { "type":"REQUEST_SCENE"}', 7)
    villa_send(sock, str, 0x0B)
end

function villa_info_attribute(sock)
    local t = {}
    t.type = 'REQUEST_ATTRIBUTE'
    local str = json.encode(t)
    -- villa_send(sock, ' { "type":"REQUEST_SCENE"}', 7)
    villa_send(sock, str, 0x0D)
end

function villa_parse(str)
    local guid = string.byte(str, 1)
    if guid ~= 0xaa then
        print("cannot find 0xaa")
        return false
    else
        local chk = 0;
        for i = 2, #str do
            chk = bitoper(chk, string.byte(str, i), XOR)
        end
        if chk == 0 then 
            print("checksum ok")
            local tab = {len, cmd, data}
            tab.len = str_cut_u16(str, 2,3)
            tab.cmd = str_cut_u16(str, 4,5)
            tab.data = string.sub( str, 6, #str - 1)
            -- print("get body", tab.data)
            return true, tab
        else
            print("check error")
            return false
        end
    end
end

function villa_recv(sock)
    local response, receive_status = sock:receive(1)
    -- hex_dump(response)
    if receive_status == "closed" then return end
    print(response)
    if not response then return end
    if string.byte(response, 1) == 0xaa then
        print("get 0xaa")
        local str = ""
        for i = 1, 4 do
            local response, receive_status = sock:receive(1)
            -- hex_dump(response)
            str = str .. response
        end
        return true, str_cut_u16(str, 1, 2), str_cut_u16(str, 3, 4)
    end
end

function villa_client(host, port, to)
    -- print("fun: "..debug.getinfo(1).name, "@line: "..debug.getinfo(1).currentline)
    if true then
        local socket = require("socket")
        local tcp = socket.tcp()
        local sock = assert(socket.connect(host, port))
        sock:settimeout(0)
        
        print("Press enter after input something:")
        
        local t = {len, cmd, data, chk}
    
        local input, recvt, sendt, status
        while true do
    
            -- socket.sleep(2)
            -- villa_heart(sock)
            recvt, sendt, status = socket.select({sock}, nil, to)
            if #recvt > 0 then
                local ret, len, cmd = villa_recv(sock)
                local data, chk
                if ret and len > 0 then
                    data = sock:receive(len)
                    chk = string.byte(sock:receive(1), 1)
                    local res_item = json.decode(data)
                    if res_item.type then
                        print("get data type "..res_item.type,"data ".. data)
                        if SmartCtrl[res_item.type] then
                            print("data callback ")
                            coroutine.yield("dispatch ok", sock, SmartCtrl[res_item.type](data) )
                        end
                    end    
                end
                recvt, sendt, status = socket.select({sock}, nil, 1)
            else
                coroutine.yield("timeout", sock)
            end
        end
    -- end
    else
    -- if false then
        local h = require("nixio")
        local sock = nixio.socket("inet", "stream")
        if not sock:connect(host, port) then
            sock:close()
            return false
        end
        sock:setopt("socket", "rcvtimeo", to)
        local tmp
        while true do
            villa_heart(sock)
            tmp = sock:recv(128)
            if tmp == false then
                sock:close()
                return false, "timeout"
            end
            print(tmp)
            ret, tab = villa_parse(tmp)
            if ret then
                print(tab.cmd, tab.len, tab.data)
                local item = json.decode(tab.data)
                if item.type then
                    print(item.type)
                end
    
            end
            socket.sleep(2)
        end
    
    end
end


-- print (bitoper(1, 2, XOR))