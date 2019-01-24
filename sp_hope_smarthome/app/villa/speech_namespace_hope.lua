#!/usr/bin/lua
package.path = '/tmp/lua/?.lua'
local json = require("cjson")
require("villa_common")
local player = hpio.player
local screen = hpio.screen
local ch_num = hpio.ch_num

local dev_path = '/tmp/http_dev.json'

local hope_dev = {
    {
        type = "AIR_CONTIONER",
        cata = '100005',
        mode = {
            ['制冷'] = 'COLD',
            ['制热'] = 'HEAT',
            ['除湿'] = '',
            ['自动'] = 'AUTO',
        },
        attribute = {
            ['风速'] = {
                name = 'WIND',
                value = {
                    {
                        ['高'] = 'HIGH',
                        ['中'] = 'MEDIUM',
                        ['低'] = 'LOW',
                        ['自动'] = 'AUTO',
                    }
                },
            },
            ['温度'] = {
                name = 'TEMPERATURE',
            },
        }
    },
    {
        type = "TV",
        cata = '100006',
        attribute = {
            ['音量'] = {
                name = 'VOLUME',
            },
            ['频道'] = {
                name = 'CHANNEL',
                value = {
                    -- {
                    --     ['-1'] = 'PREV',
                    --     ['+1'] = 'NEXT',
                    -- },
                    {
                        ['+'] = 'NEXT',
                        ['-'] = 'PREV',
                    }
                },
            }
        }
    },
    {
        type = 'LIGHT',
        cata = '100001'
    },
    {
        type = 'SOCKET',
        cata = '100002'
    },
}

local hope_value = {
    ['+'] = 'PLUS',
    ['-'] = 'MINUS',
}

local hope_value_to = {
    ['最大'] = 'MAX',
    ['最小'] = 'MIN',
}

local function _get_attribute( param , tab)
    
    if not tab.attribute then return end
        
    for k, v in pairs(tab.attribute) do
        if param[k] then
            for _k, _v in pairs(v.value) do
                if param[k]:find(_k) then
                    return v.name ,_v, nil
                end
            end
            for _k, _v in pairs(hope_value) do
                if param[k]:find(_k) then
                    return v.name, _v, nil
                end
            end
            for _k, _v in pairs(hope_value_to) do
                if param[k]:find(_k) then
                    return v.name, 'TO', _v
                end
            end
            if tonumber(param[k]) then
                return v.name, 'TO', tostring(param[k])
            end
        end
    end
    return nil, nil, nil
end

local function _get_action( str )
    if not (str and #str > 0) then return nil end
    local hope_act = {
        ['开'] = 'ON',
        ['关'] = 'OFF',
    }
    for k, v in pairs(hope_act) do
        if str:find(k) then
            return v
        end
    end
    return nil
end


function hope_smarthome_devcice( param , ori )
    local info_list = {}
    local ret = {}
    local dev_name = ''

    -- local dev_name = param['设备名称']
    
    -- local s, e = ori:find(param['设备名称'])
    -- if s then
    --     local str = ori:sub(e + 1)
    --     local dev_id = ch_num.convert_force(str)
    --     if dev_id >= 0 then
    --         dev_name = dev_name..tostring(dev_id)
    --     end
    -- end
    dev_name = getdevid(param['设备名称'], ori)
    print(dev_name)

    if file_exists(dev_path) then
        info_list = json.decode(cut_str_to_json(io.input(dev_path):read("*a")))
        -- dump_tab(info_list)
    else return nil end
    
    local attr_tab = {}
    for k, v in ipairs(info_list.object.device) do
        -- dump_tab(v)
        if dev_name == v.deviceName then
            -- print('find '..v.deviceName..'\tcata'..v.hopeCata)
            for k_t, v_t in pairs(hope_dev) do
                dump_tab(v_t)
                if tonumber(v_t.cata) == tonumber(v.hopeCata) then
                    attr_tab = v_t
                    break
                end
            end
            break
        end
    end

    if attr_tab.cata then
        -- print("find")
        ret.action = _get_action(param['操作'])
        -- print(ret.action)
        local attribute, action, value = _get_attribute(param, attr_tab)
        if attribute then
            ret.attribute = attribute
            ret.action = action
            ret.value = value
        else
            if ret.action then
                ret.attribute = 'POWER'
            end
        end
        ret.device = dev_name
        return ret
    else
        -- print("not find")
        return nil
    end

end


function hope_smarthome( param , ori )
    if param['设备名称'] then
        return hope_smarthome_devcice(param, ori)
    end
end