#!/usr/bin/lua

speech_state = {
    ['开'] = 'STATE_ON',
    ['关'] = 'STATE_OFF',
    ['停'] = 'STATE_STOP',
}

speech_mode = {
    ['制冷'] =      'MODE_COOL',
    ['制热'] =      'MODE_HEAT',
    ['送风'] =      'MODE_FAN',
    ['抽湿'] =      'MODE_DRY',
    ['除湿'] =      'MODE_DRY',
    ['干燥'] =      'MODE_DRY',
    ['自动'] =      'MODE_AUTO',
    ['手动'] =      'MODE_MANUAL',
    ['睡眠'] =      'MODE_SLEEP',
    ['高风'] =      'MODE_WIND_HIGH',
    ['中风'] =      'MODE_WIND_MID',
    ['低风'] =      'MODE_WIND_LOW',
    ['自动风'] =    'MODE_WIND_AUTO',
    ['自然风'] =    'MODE_WIND_NATURAL',
    ['正常风'] =    'MODE_WIND_NORMAL',
    ['静音风'] =    'MODE_WIND_MUTE',
    ['睡眠风'] =    'MODE_WIND_SLEEP',
    ['舒适风'] =    'MODE_WIND_COMFORTABLE',
    ['播放'] =      'MODE_PLAY',
    ['暂停'] =      'MODE_PAUSE',
    ['静音'] =      'MODE_MUTE',
    ['下一首'] =    'MODE_NEXT',
    ['上一首'] =    'MODE_PREVIOUS',
    ['摇头'] =      'MODE_SWING_ANGLE',
    ['摆风'] =      'MODE_SWING_ANGLE',
    ['左右摆风'] =  'MODE_SWING_LEFT_RIGHT',
    ['上下摆风'] =  'MODE_SWING_UP_DOWN',
    ['空气净化'] =  'MODE_PURFIER',
    ['除霜'] =      'MODE_FROST',
    ['节能'] =      'MODE_ECO',
    ['经济'] =      'MODE_ECO',
    ['童锁'] =      'MODE_LOCK',
    ['定时'] =      'MODE_TIMING',
}

speech_attribute = {
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
}

speech_attr_val = {
    ['max'] =   'VALUE_MAX',
    ['min'] =   'VALUE_MIN',
    ['else'] =  'VALUE_LITTLE',
}

speech_act = {
    ['+'] = {
        act = 'ACTION_ADD',
        val = 'VALUE_LITTLE',
        a_voice = '提高',
        v_voice = '',
    },
    ['-'] = {
        act = 'ACTION_REDUCE',
        val = 'VALUE_LITTLE',
        a_voice = '降低',
        v_voice = '',
    },
    ['max'] = {
        act = 'ACTION_TO',
        val = 'VALUE_MAX',
        a_voice = '设为',
        v_voice = '最大',
    },
    ['min'] = {
        act = 'ACTION_TO',
        val = 'VALUE_MIN',
        a_voice = '设为',
        v_voice = '最小',
    },
    ['+1'] = {
        act = 'ACTION_ADD',
        a_voice = '打开',
        v_voice = '下一个',
    },
    ['-1'] = {
        act = 'ACTION_REDUCE',
        a_voice = '打开',
        v_voice = '上一个',
    },
    ['else'] = 'ACTION_TO',
}

speech_color = {
    ['白'] = 'VALUE_COLOR_WHITE',
    ['红'] = 'VALUE_COLOR_RED',
    ['橙'] = 'VALUE_COLOR_ORANGE',
    ['黄'] = 'VALUE_COLOR_YELLOW',
    ['绿'] = 'VALUE_COLOR_GREEN',
    ['青'] = 'VALUE_COLOR_CYAN',
    ['蓝'] = 'VALUE_COLOR_BLUE',
    ['紫'] = 'VALUE_COLOR_PURPLE',
    ['粉'] = 'VALUE_COLOR_PINK',
}

function converse_tab( tab )
    local t = {}
    for k, v in pairs(tab) do
        t.v = k
    end
    return t
end

function halo_smarthome_dev( param, ori )
    local info_list = {}
    local ret = {}
    local dev_name = param['设备名称']
    local s, e = ori:find(param['设备名称'])
    if s then
        local str = ori:sub(e + 1)
        local dev_id = ch_num.convert_force(str)
        if dev_id >= 0 then
            dev_name = dev_name..tostring(dev_id)
        end
    end
end