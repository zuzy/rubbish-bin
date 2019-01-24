#!/usr/bin/lua

-- package.path = '/tmp/lua/?.lua'
-- require('num2ch')
local ch_num = hpio.ch_num

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
    if not u then
        print("input is nill")
        return nill
    end
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

function str_cut_u16(str, a, b)
    return string.byte(str, a) * 256 + string.byte(str, b)
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
    local length = 0
    local l = 0
    for i = 1, #str do
        local byte = string.byte(str, i)
        if l == 0 then
            l = byte_len(byte)
            length = length + 1
        end
        l = l - 1
    end
    return length
end

function cut_str_to_json( str )
    if type(str) ~= 'string' then
        return nil
    end
    if str:find('{') then
        str = str:sub(str:find('{'), -1):reverse()
        if str:find('}') then
            return str:sub(str:find('}'), -1):reverse()
        end
    end
end

local zone = -1
function dump_tab( tab )
    if not tab then return end
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

function bool_to_number(value)
    return value and 1 or 0
end
  

local key_model = {
    '模式', '场景',
}

function filter_scene(str)
    for i, v in ipairs(key_model) do
        str = str:gsub(v, "+%1")
    end
    return str
end

function cut_scene( str )
    local a = str:find('+')
    if a > 1 then
        return str:sub(1, a - 1)
    else
        return str
    end
end

function undo_scene( str )
    return str:gsub('+','')
end


function num_to_tts(num)
    local scale = {"",'十', '百', '千', '万'}
    local base = {'一','二','三','四','五','六','七','八','九'}
    local tts = ""
    local flag = ""
    if num < 0 then
        flag = "负"
        num = -num
    end
    for i, v in ipairs(scale) do
        if num == 0 then break end
            local n = math.fmod( num,10 )
        if n > 0 then tts = scale[i]..tts end

        tts = ((n > 0 and base[n]) or tostring(n)).. tts
        -- tts = tostring(n)..tts
        num = math.modf( num / 10 )
    end
    tts = tts
    if #tts > 0 then
        tts = tts:gsub('0',' ')
        tts = tts:reverse()
        local i,j = tts:find("(%s+)")
        if i == 1 then 
            tts = tts:sub(j+1) 
        end
        tts = tts:reverse()
        i, j = tts:find('一十')
        if i == 1 then
            j = tts:find('十')
            tts = tts:sub(j)
        end
    else
        tts = ' '
    end

    tts = tts:gsub('(%s+)', '零')
    return flag..tts
end
  
  

-- local function url2name( uri )
--     if type(uri) == 'string' then
--         local s = uri:reverse()
--         return s:match("%.([^/]*)"):reverse()
--     end
--     return nil
-- end

function url2name( uri )
    if type(uri) == 'string' then
        return uri:match("%/([^/]*)%.")
    end
    return nil
end
  
-- function chk_num_persent( str )
--     if tonumber(str) then
--         return tonumber(str), 1
--     end
--     if str:find('%d+%s*%%') then
--         return str:sub(str:find('%d+')), 2
--     else
--         return str, 0
--     end
-- end

function chk_num_persent(str)
    if tonumber(str) then 
        return tonumber(str), 1
    end
    if str:find('%d+%s*%%') then 
        return tonumber(str:sub(str:find('%d+'))), 2
    else
        return str, 0
    end
end

function sep_num( str )
    if type(str) == 'string' and str:find('^[-+]%s*%d+') then
        return str:sub(str:find('[-+]')), tonumber(str:sub(str:find('%d+')))
    end
end


local pos = {
    "",
    "十",
    "百",
    '千',
}

local pat = {
    '一',
    '二',
    '三',
    '四',
    '五',
    '六',
    '七',
    '八',
    '九',
    '零',
}

local function cut_zero( input )
    str = string.gsub( input,"零"," ")
    str = string.reverse( str )
    local s, e = string.find( str,"%s+")
    if s and s == 1 then
        str = string.sub( str, e+1)
    end
    str = string.reverse( str )
    str = string.gsub( str, "%s+", "零" )
    return str
end

function num2char( num )
    if num < 0 or num > 999 then
        return nil, nil
    end
    if num == 0 then
        return '零', '零'
    end
    local str = tostring(num)
    local ret = ""
    local s_ret = ""
    for i = #str, 1, -1 do
        local tmp = math.floor( num / (10 ^ (i-1)))
        if tmp > 0 then
            if i == 2 and tmp == 1 then
                ret = ret..pos[i]
            else
                ret = ret..pat[tmp]..pos[i]
            end
            s_ret = s_ret..pat[tmp]
        else 
            ret = ret .. '零'
            s_ret = s_ret..'零'
        end
        num = math.fmod( num,(10 ^ (i - 1)) )
    end
    return cut_zero(ret), s_ret
    -- return ret;
end


-- get devid return dev with id before
function getdevid( dev, ori )
    local dev_name = dev
    if dev and ori then
        local s, e = ori:find(dev)
        if s then
            local str = ori:sub(e + 1)
            local dev_id = ch_num.convert_force(str)
            if dev_id >= 0 then
                dev_name = dev_name..tostring(dev_id)
            end
        end
    end
    return dev_name
end

function file_exists(file)
    local f = io.open(file, "rb")
    if f then f:close() end
    return f ~= nil
end