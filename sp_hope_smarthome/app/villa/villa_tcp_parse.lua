#!/usr/bin/lua
local json = require("cjson")

function heart_beat(arg)
    print("fun: "..debug.getinfo(1).name, "@line: "..debug.getinfo(1).currentline)
    print("heart beat")
    return true
end

function re_device(arg)
    print("response dev")
    local item = json.decode(arg)
    local data = item.data
    for i, v in ipairs(data) do
        print(i, v.id, v.name, v.type, v.state, v.roomId, v.floorId)
    end
    return true
end

function re_scene(arg)
    print("response scene")
    local item = json.decode(arg)
    local data = item.data
    for i, v in ipairs(data) do
        print(i, v.id, v.name, v.roomId, v.floorId)
    end
    return true
end

function re_room(arg)
    print("response room")
    local item = json.decode(arg)
    local data = item.data
    for i, v in ipairs(data) do
        print(i, v.id, v.name)
    end
    return true
end

function re_floor(arg)
    print("response floor")
    local item = json.decode(arg)
    local data = item.data
    for i, v in ipairs(data) do
        print(i, v.id, v.name)
    end
    return true
end

function re_attribute(arg)
    print("response attribute")
    return true
end

function re_control(arg)
    print("ctrl!!")

    
    return true
end

function req_music_ctrl(arg)
    print("music ctrl")
    local item = json.decode(arg)
    local data = item.data
    print("effect:"..tostring(data.effect), 
    "source:"..tostring(data.source),  
    "volume:"..tostring(data.volume), 
    "mute:"..tostring(data.mute), 
    "mode:"..tostring(data.mode), 
    "playstate:"..tostring(data.playstate), 
    "process:"..tostring(data.process), 
    "songid:"..tostring(data.songid))
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
}

