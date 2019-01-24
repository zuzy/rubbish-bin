#!/usr/bin/lua
package.path = '/tmp/lua/?.lua'
local json = require("cjson")

require("villa_common")

local player = hpio.player
local screen = hpio.screen
local ch_num = hpio.ch_num


local base_path = '/mnt/udisk/'
local conf_file = {
    dev_file = 'device.lua',
    sce_file = 'scene.lua',
    flr_file = 'floor.lua',
    rms_file = 'rooms.lua'
}


device_type = {
    ['灯'] = 'LISGHT',
    ['调光灯'] = 'DIMMING_LIGHT',
    ['插座'] = 'SOCKET',
    ['窗帘'] = 'CURTAIN',
    ['风扇'] = 'ELECTRIC_FAN',
    ['空调'] = 'AIR_CONDITION',
    ['电视'] = 'TV',
    ['传感器'] = 'SENSOR',
    ['其他'] = 'OTHER',
}

device_state = {
    ['开'] = 'STATE_ON',
    ['关'] = 'STATE_OFF',
    ['停'] = 'STATE_STOP',
}

device_mode = {
    ['制冷'] = 'MODE_COOL',
    ['制热'] = 'MODE_HEAT',
    ['抽湿'] = 'MODE_DRY',
    ['干燥'] = 'MODE_DRY',
    ['自动'] = 'MODE_AUTO',
    ['睡眠'] = 'MODE_SLEEP',
    ['除霜'] = 'MODE_FROST',
    ['节能'] = 'MODE_ECO',
    ['经济'] = 'MODE_ECO',
    ['净化'] = 'MODE_PURIFIER',
    ['换气'] = 'MODE_PURIFIER',
}

device_attribute = {
    ['风速'] =  'ATTRIBUTE_WIND_SPEED',
    ['温度'] =  'ATTRIBUTE_TEMPERATURE',
    ['湿度'] =  'ATTRIBUTE_HUMIDITY',
    ['档位'] =  'ATTRIBUTE_GEAR',
    ['亮度'] =  'ATTRIBUTE_BRIGHTNESS',
    ['颜色'] =  'ATTRIBUTE_COLOR',
    ['色温'] =  'ATTRIBUTE_COLOR_TEMP',
    ['音量'] =  'ATTRIBUTE_VOLUME',
    ['声音'] =  'ATTRIBUTE_VOLUME',
    ['频道'] =  'ATTRIBUTE_CHANNEL',
    ['节目'] =  'ATTRIBUTE_CHANNEL',
    ['台'] =    'ATTRIBUTE_CHANNEL',
    ['小时'] =  'ATTRIBUTE_HOUR',
    ['分钟'] =  'ATTRIBUTE_MINUTE',
    ['秒钟'] =  'ATTRIBUTE_SECOND',
    ['摇头'] = 'ATTRIBUTE_SWING_ANGLE',
    ['摆风'] = 'ATTRIBUTE_SWING_ANGLE',
    ['静音'] = 'ATTRIBUTE_MUTE',
    ['童锁'] = 'ATTRIBUTE_LOCK',
}

device_act = {
    ['+'] = {
        act = 'ACTION_ADD',
    },
    ['-'] = {
        act = 'ACTION_REDUCE',
    },
    ['开'] = {
        act= 'ACTION_OPEN',
    },
    ['关'] = {
        act = 'ACTION_CLODE',
    },
    ['max'] = {
        act = 'ACTION_TO',
        val = 'VALUE_MAX',
    },
    ['min'] = {
        act = 'ACTION_TO',
        val = 'VALUE_MIN',
    },
}

local function location_dev( name, floor, room )
    if not file_exists(base_path..conf_file.dev_file) then return end
    local json_tmp = json.decode(io.input(base_path..conf_file.dev_file):read("*a"))

    local type = (floor and 1 or 0) + (room and 2 or 0)
    print('type', type)
    local ids = {}
    local val_tmp = {}
    for i, v in ipairs(json_tmp) do
        -- print(v.name_id, name)
        if v.name_id == name then
            return 1, v
        elseif v.name:find(name) then
            print(v.floor, v.room, v.name)
            if type == 1 then
                if floor == v.floor then 
                    table.insert(ids, v.id) 
                    val_tmp = v
                end
            elseif type == 2 then
                if room == v.room then 
                    table.insert(ids, v.id) 
                    val_tmp = v
                end
            elseif type == 3 then
                if room == v.room and floor == r.floor then 
                    table.insert(ids, v.id)
                    val_tmp = v
                end
            else
                table.insert(ids, v.id)
                val_tmp = v
            end
        end
    end
    print('ids ----------')
    dump_tab(ids)
    print('ids ----------')
    
    if table.maxn(ids) == 1 then
        return 1, val_tmp
    elseif table.maxn(ids) == 0 then
        return 0, nil
    else 
        return 2, ids
    end
end

local function halo_smarthome_device( param, ori )
    if not file_exists(base_path..conf_file.dev_file) then 
        print(base_path..conf_file.dev_file, 'not exist')
        return
    end
    local dev_tab = {}
    -- dev_tab.dev_name = getdevid(param['设备名称'],ori)
    dev_tab.dev_name = param['设备名称']
    print('halo get dev', dev_tab.dev_name)
    
    if param['楼层'] then dev_tab.floor = param['楼层'] end
    if param['位置'] then dev_tab.room = param['位置'] end



    if param['操作'] then dev_tab.act = param['操作'] end
    if param['数量'] then dev_tab.num = param['数量'] end
    if param['模式'] or param['场景'] then
        dev_tab.mode = param['模式'] or param['场景'] 
    end
    dump_tab(dev_tab)
    local t, val = location_dev(dev_tab.dev_name, dev_tab.floor, dev_tab.room)

    for k, v in pairs(device_attribute) do
        if param[k] then
            dev_tab.attribute = k
            local num = tonumber(param[k])
            if num then
                dev_tab.act = 'ACTION_TO'
                dev_tab.attributeValue = param[k]
            else
                for _k, _v in pairs(device_act) do
                    if v:find(_k) then
                        dev_tab.act = _v.act
                        if _v.val then
                            dev_tab.attributeValue = _v.val
                        end
                        break
                    end
                end
            end
        end
    end
    print('dev_tab==========')
    dump_tab(dev_tab)
    print('dev_tab==========')

end

function halo_smarthome( param, ori )
    if param['设备名称'] then
        halo_smarthome_device(param, ori)
    end
end
