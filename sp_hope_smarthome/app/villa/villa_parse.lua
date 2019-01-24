#!/usr/bin/lua
local json = require("cjson")
require("villa_io")
require("villa_common")

local player = hpio.player

function heart_beat(item)
    print("heart beat")
    return true
end

function re_device(item)
    print("response dev")
    local data = item.data
    local str = '$device=('
    for i, v in ipairs(data) do
        print(i, v.id, v.name, v.type, v.state, v.roomId, v.floorId)
        str = str..v.name..'|'
    end
    str = str:sub(1, #str-1)
    str = str..")\n"
    write_json_file(str)
    return true
end

function re_scene(item)
    print("response scene")
    local data = item.data
    for i, v in ipairs(data) do
        print(i, v.id, v.name, v.roomId, v.floorId)
    end
    return true
end

function re_room(item)
    print("response room")
    local data = item.data
    local str = '$room=('
    for i, v in ipairs(data) do
        print(i, v.id, v.name)
        str = str..v.name..'|'
    end
    str = str:sub(1, #str - 1)
    str = str..")\n"
    print("str"..str)
    write_init(str)
    -- write_json_file(str)
    return true
end

function re_floor(item)
    print("response floor")
    local data = item.data
    local str = '$floor=('
    for i, v in ipairs(data) do
        print(i, v.id, v.name)
        str = str..v.name..'|'
    end
    str = str:sub(1, #str-1)
    str = str..")\n"
    write_json_file(str)
    return true
end

function re_attribute(item)
    print("response attribute")
    return true
end

function re_control(item)
    print("ctrl!!")

    
    return true
end

local t_loop_mode = {
    random = 4,
    single = 2,
    cycle = 3,
    list = 1,
}

function req_music_ctrl(item)
    print("music ctrl")
    local data = item.data
    print("effect:"..tostring(data.effect), 
    "source:"..tostring(data.source),  
    "volume:"..tostring(data.volume), 
    "mute:"..tostring(data.mute), 
    "mode:"..tostring(data.mode), 
    "playstate:"..tostring(data.playstate), 
    "process:"..tostring(data.process), 
    "songid:"..tostring(data.songid))
    if data.volume == "inc" then 
        player.vol_ctrl(1)
    else 
        player.vol_ctrl(0)
    end

    if data.mute then
        player.mute(1)
    end

    if data.mode == 'random' then
        print("loop mode is "..tostring(t_loop_mode[data.mode]))
        player.loop(t_loop_mode[data.mode])
    end

    return true
end

function req_music_specify( item )
    print("music specify")
    local data = item.data
    local songs = data.songs
    for i, v in ipairs(songs) do
        print("type: "..tostring(v.type), "title: "..tostring(v.title), "progress: "..tostring(v.progress))
    end
    return true
end

SmartCtrl = {
    ['RESPONSE_HEART_BEAT'] =   heart_beat,
    ['RESPONSE_DEVICE'] =       re_device,
    ['RESPONSE_SCENE'] =        re_scene,
    ['RESPONSE_ROOM'] =         re_room,
    ['RESPONSE_FLOOR'] =        re_floor,
    ['RESPONSE_ATTRIBUTE'] =    re_attribute,
    ['RESPONSE_CONTROL'] =      re_control,
    ['REQUEST_MUSIC_CONTROL'] = req_music_ctrl,
    ['REQUEST_SPECIFY_PLAY'] =  req_music_specify,
}

function cut_str_to_json( input )
    local n
    for i = #input, 1, -1 do
        if input:byte(i) == string.byte('}') then
            n = i
            break
        end
    end
    return input:sub(1, n)
end

function lua_c_parse( input_str )
    local str = cut_str_to_json(input_str)
    local item =json.decode(str)
    if item then
        for k, v in pairs(item) do
            print(k, v)
        end
        if item.type then
            print( item.type )
            SmartCtrl[item.type](item)
            return item.type
        end
    end
    return "bye"
end
