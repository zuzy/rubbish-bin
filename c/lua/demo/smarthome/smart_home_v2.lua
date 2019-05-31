#!/usr/bin/lua
-- local json = require("cjson")
-- local nix = require("nixio")
-- local sock = require("socket")


-- local core = {}
-- core.ip = "192.168.31.246"
-- core.port = 8888

-- core.rcvTimeout = 20

-- client.lua
require("socket")
local json = require("cjson")
OR, XOR, AND = 1, 3, 4

function bitoper(a, b, oper)
   local r, m, s = 0, 2^52
   repeat
      s,a,b = a+b+m, a%m, b%m
      r,m = r + m*oper%(s-a-b), m/2
   until m < 1
   return r
end

-- print(bitoper(6,3,OR))
-- print(bitoper(6,3,XOR))
-- print(bitoper(6,3,AND))

function u16_to_str(u)
    local s = math.modf(u/256)
    return string.char(s)..string.char(u%256)
end

function hex_dump(str)
    if str then
        for i = 1, #str do
            print(string.format("%02X ", string.byte(str, i)))
        end
    end
end

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
end

function villa_heart(sock)
    villa_send(sock,'{"type":"REQUEST_HEART_BEAT"}', 1)
end

function str_cut_u16(str, a, b)
    return string.byte(str, a) * 256 + string.byte(str, b)
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

if false then
    local h = require("nixio")
    local core = {}
    core.ip = "192.168.31.246"
    core.port = 8888
    core.rcvTimeout = 10
    local sock = nixio.socket("inet", "stream")
    if not sock:connect(core.ip, core.port) then
        sock:close()
        return false
    end
    sock:setopt("socket", "rcvtimeo", core.rcvTimeout)
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

if true then
    local socket = require("socket")
    local tcp = socket.tcp()
    local host = "192.168.31.246"
    -- local port = 12345
    local port = 8888
    local sock = assert(socket.connect(host, port))
    -- local sock = assert(tcp.connect(host, port))    
    sock:settimeout(0)
    
    print("Press enter after input something:")
    
    local t = {0x12, 0x23, 0xaa, 0xff}

    local input, recvt, sendt, status
    while true do

        -- socket.sleep(2)
        villa_heart(sock)
        recvt, sendt, status = socket.select({sock}, nil, 1)
        while #recvt > 0 do
            print("recv t > 0")
            local response, receive_status = sock:receive();
            hex_dump(response)
            if receive_status ~= "closed" then
                print("not close!")
                if response then
                    print("resp is not nill")
                    print(response)
                    ret, tab = villa_parse(response)
                    print(ret, tab.data)

                end
                recvt, sendt, status = socket.select({sock}, nil, 1)
                print("rep end")
            else
                sock:close()
                break
            end
            print("recv t end")
        end
        
        -- socket:select(nil, nil, 2)

        -- local tt = {0xaa, len, cmd, data, chk}
        -- tt.data = '{"type":"REQUEST_HEART_BEAT"}'
        -- tt.len = #tt.data
        -- tt.cmd = 1
        -- local str = u16_to_str(tt.len)..u16_to_str(tt.cmd)..tt.data
        -- tt.chk = 0;
        -- for i = 1, #str do
        --     tt.chk = bitoper(tt.chk, string.byte(str,i), XOR)
        --     -- chk = chk ~ string.byte(str. i)
        -- end
        -- str = string.char(tt[1]) .. str .. string.char(tt.chk)
        -- sock:send(str)

    end
end
