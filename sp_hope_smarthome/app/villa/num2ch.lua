#!/usr/bin/lua
-- local ch_num = "灯 一百二十八"

-- local pattern = '[一二三四五六七八九百十]+'

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


-- package.cpath = "./?.so"
-- local ch2num = require("ch2num")
-- local str = "灯一百sdfdd"
-- local ret = ch2num.ch_to_num(str)
-- print(str, ret)

-- print(num2char(123))
-- print(num2char(23))
-- print(num2char(120))
-- print(num2char(10))
-- print(num2char(102))
-- print(num2char(100))
-- print(num2char(1023))
-- print(num2char(1003))
-- print(num2char(1030))
-- print(num2char(1000))
-- print(num2char(100))
-- print(num2char(13))
-- print(num2char(23))
-- print(num2char(999))
-- print(num2char(9999))
-- print(num2char(100))
