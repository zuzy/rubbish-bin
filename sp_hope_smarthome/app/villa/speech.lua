#!/usr/bin/lua
package.path = '/tmp/lua/?.lua'
local json = require("cjson")
require("villa_common")
require("speech_namespace")
require("speech_namespace_hope")
require("speech_halo")

local para_file = '/mnt/udisk/speech_log.lua'

local function write_to_file( str )
    local f = io.open(para_file, "a+")
    str = str:gsub("(%s+)", "")
    f:write(str.."\n")
end

local base_path = '/mnt/udisk/'
local conf_file = {
    dev_file = 'device.lua',
    sce_file = 'scene.lua',
    flr_file = 'floor.lua',
    rms_file = 'rooms.lua'
}


  
function string_from(file)
    if not file_exists(file) then return "" end
    -- local str = ""
    -- local f = io.input(file)
    -- local str = f:read('*a')
    local str = io.input(file):read('*a')
    return str
end
    

function speech_to_villa( str )
    local item = json.decode(str)
    local tab = {}
    tab.type = 'REQUEST_CONTROL'
    tab.original = item.original
    item.original = nil
    local arr = {}
    arr[1] = item
    tab.data = arr
    local s = json.encode(tab)
    print(s)
    local tt = {}
    tt[tab.original] = tab.data
    print(json.encode(tt))
    return s
end


function speech_to_halo( type, str )
    local item = json.decode(str)
    item.original = nil
    local s = ""
    local tab = {}

    print("speech to halo")
    dump_tab(item)
    print("--------------------")

    if type == 1 then

        tab.cmd = 'devicecontrol'
        
        local file_item = {}
        if file_exists(base_path..conf_file.dev_file) then
            file_item = json.decode(string_from(base_path..conf_file.dev_file))
        end
        if table.maxn(file_item) == 0 then file_item = nil end

        if item.id then 
            item.ids = nil 
            local arr = {}
            arr[1] = item
            tab.data = arr
            s = json.encode(tab).."\n"
            return s
        end

        local _ids = item.ids
        item.ids = nil
        if _ids then
            for i, v in ipairs(_ids) do
                item.id = tonumber(v)
                if file_item then
                    for i, v in ipairs(file_item) do
                        if tonumber(v.id) == tonumber(item.id) then
                            item.name = v.name
                            item.roomId = v.roomId
                            item.floorId = v.floorId
                            item.type = v.type
                            break
                        end
                    end
                end
                local arr = {}
                arr[1] = item

                tab.data = arr
                s = s..json.encode(tab).."\n"
            end
        end
    elseif type == 2 then
        tab.cmd = 'scenecontrol'
        local arr = {}
        arr[1] = item
        tab.data = item
        if item.ids then item.ids = nil end
        s = json.encode(tab).."\n"
    end
    return s
end

function speech_dispatch( str )
    print("speech dispatch "..str)
    local item = json.decode(str)

    local halo_tab = hope_smarthome( item.request.param , item.input )
    print('---------------')
    dump_tab(halo_tab)
    print('---------------')

    halo_smarthome( item.request.param , item.input )

    return 2, "", '抱歉,没找到相应场景'
end

function fucn()

    local tab = {}
    local tts = "马上为您"
    local names = ""
    local _attr = {}
    
    if item.request.param then
        local ori = item.input
        if ori then tab.original = ori end

        local _time = item.request.param['时间']
        local _date = item.request.param['日期']
        tab.time = (_date and _time and (_date..'+'.._time)) or _date or _time
        
        local flr = item.request.param['楼层']
        if flr then tab.floorId = flr end

        local rm = item.request.param['位置']
        if rm then tab.roomId = rm end

        local n = item.request.param['设备名称']
        if n then tab.name = n end

        local mode = item.request.param['模式']
        local scene = item.request.param['场景']

        if scene or (mode and tab.name == nil) then
            local sce_arr = {}
            if file_exists(base_path..conf_file.sce_file) then
                print("inside")
                sce_arr = json.decode(io.input(base_path..conf_file.sce_file):read("*a"))
                print("helo")
                if table.maxn(sce_arr) ~= 0 then
                    for i, v in ipairs(sce_arr) do
                        if (mode or scene):find(cut_scene(v.name))then
                            tab.id = tonumber(v.id)
                            tab.scene = undo_scene(v.name)
    
                            tts = tts.."切换到"..tab.scene..'~'
    
                            if v.roomId then tab.roomId = v.roomId end
                            if v.floorId then tab.floorId = v.floorId end
                            local r = json.encode(tab)
                            write_to_file(str.." ----~> "..r.."\n")
                            return 2, r, tts
                        end
                    end
                end
            end
            return 2, "", '抱歉,没找到相应场景'
        end
        


        if speech_mode[mode] then 
            tab.mode = speech_mode[mode]
            _attr.mode = mode
        else
            tab.mode = mode 
            for k, v in ipairs(speech_mode) do
                if mode:find(k) then
                    tab.mode = v
                    _attr.mode = mode
                    break
                end
            end
        end

        
        local act = item.request.param['操作']
        if act then
            for k, v in pairs(speech_state) do
                if act:find(k) then
                    tab.state = v
                    break
                end
            end
        end

        -- local conv_attr = converse_tab(speech_attribute)
        for k, v in pairs(speech_attribute) do
            local mm = item.request.param[k]
            if mm then
                _attr.name = k
                print('_attr.name ', _attr.name)
                tab.attribute = v
                if v == 'ATTRIBUTE_COLOR' then
                    for t_k, t_v in pairs(speech_color) do
                        if mm:find(t_k) then
                            tab.action = 'ACTION_TO'
                            tab.attributeValue = t_v
                            _attr.act = '设为'
                            _attr.num = t_k..'色'
                            break
                        end
                    end
                else

                    if speech_act[mm] then
                        tab.action = speech_act[mm].act
                        _attr.act = speech_act[mm].a_voice
                        _attr.val = speech_act[mm].v_voice
                        tab.attributeValue = speech_act[mm].val
                    else
                        local num, persent = chk_num_persent(mm)
                        print('persent', persent)
                        tab.action = speech_act['else']
                        tab.attributeValue = tostring(num)..((persent == 2) and '%' or '')
                        _attr.act = '设为'
                        print('num is ', num)
                        _attr.num = ((persent == 2) and '百分之' or '') .. num_to_tts(num)

                    end
                end
                break
            end
        end
        _attr.act = _attr.act or act
    end
    tab.ids = {}
    if tab.name and file_exists(base_path..conf_file.dev_file) then 
        -- local s = string_from(base_path..conf_file.dev_file)
        -- local json_tmp = json.decode(s)
        local json_tmp = json.decode(string_from(base_path..conf_file.dev_file))
        -- print('ssss')
        local index = 1
        for i, v in ipairs(json_tmp) do
            -- dump_tab(v)
            if v.name:find(tab.name) then 
                if tab.floorId and tab.roomId then
                    if tab.floorId:find(v.floor) and tab.roomId:find(v.room) then
                        tab.id  = v.id
                        names = v.name
                        tab.floorId = tonumber(v.floorId)
                        tab.roomId = tonumber(v.roomId)
                        tab.type = v.type
                        break
                    end
                else
                    if tab.floorId then
                        if type(tab.floorId) == 'string' then
                            if tab.floorId:find(v.floor) then
                                tab.ids[index] = v.id
                                names = (#names > 0 and (names..","..v.name)) or v.name
                                tab.floorId = tonumber(v.floorId)
                                index = index + 1
                            end
                        elseif type(tab.floorId) == 'number' then
                            if tab.floorId == tonumber(v.floorId) then
                                tab._ids[index] = v.id
                                names = (#names > 0 and names..","..v.name) or v.name
                                index = index +1
                            end
                        end
                    elseif tab.roomId then
                        if type(tab.roomId) == 'string' then
                            if tab.roomId:find(v.room) then
                                tab.ids[index] = v.id
                                names = (#names > 0 and names..","..v.name) or v.name
                                tab.roomId = tonumber(v.roomId)
                                index = index + 1
                            end
                        elseif type(tab.roomId) == 'number' then
                            if tab.roomId == tonumber(v.roomId) then
                                tab._ids[index] = v.id
                                names = (#names > 0 and names..","..v.name) or v.name
                                index = index + 1
                            end
                        end
                    else 
                        tab.ids[index] = v.id
                        -- if #names > 0 then
                        --     names = names..','..v.name
                        -- else
                        --     names = v.name
                        -- end
                        names = (#names > 0 and (names..","..v.name)) or v.name
                        index = index + 1
                        print(names)
                    end
                end
            elseif item.input:find(v.name) then
                print('match')
            end

        end
    end
    print("-------------------")
    dump_tab(_attr)
    print(names)
    print("-------------------")
    
    if #names > 0 then
        if _attr.name then
            print('attr name', _attr.name)
            if _attr.val then
                tts = tts..'将'..names.._attr.act.._attr.val.._attr.name
            else
                tts = tts..'将'..names..'的'.._attr.name.._attr.act.._attr.num
            end

        elseif _attr.mode then
            print('attr mode', _attr.mode)
            tts = tts..'将'..names..'设为'.._attr.mode
        elseif _attr.act then
            print('attr act', _attr.act)
            tts = tts.._attr.act..names
        else
            tts = nil
        end
    else
        tts = '抱歉～找不到该设备'
        -- tts = nil
    end
    
    print("end")
    print(tts)
    local r = json.encode(tab)
    write_to_file(str.." ----~> "..r.."\n")
    return 1, r, tts
end
