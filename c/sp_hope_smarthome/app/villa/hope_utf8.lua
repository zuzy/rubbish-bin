#!/usr/bin/lua

OR, XOR, AND = 1, 3, 4
function bitoper(a, b, oper)
    local r, m, s = 0, 2^52
    repeat
       s,a,b = a+b+m, a%m, b%m
       r,m = r + m*oper%(s-a-b), m/2
    until m < 1
    return r
end

function byte_len( byte )
    local len = 0
    for i = 7, 0, -1 do
        if bitoper(2^i, byte, AND) ~= 0 then
            len = len + 1
        else
            break
        end
    end
    if len > 0 then
        return len
    else
        return 1
    end
end

function utf8_len( str )
    -- print(#str)
    -- local tab = string.byte(str, 1, 6)
    -- -- for k, v in pairs(tab) do
    -- --     print(k, string.format("%02X", v))
    -- -- end
    -- print(string.format("%02X", tab))

    local byte
    local length = 0
    local l = 0
    for i = 1, #str do
        byte = string.byte(str, i)
        if l == 0 then
            l = byte_len(byte)
            length = length + 1
        end
        l = l - 1
    end
    -- print(length)
    return length
end

utf8_len("阿斯蒂芬飞洒地方了科技")