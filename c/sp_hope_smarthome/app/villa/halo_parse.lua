#!/usr/bin/lua
package.path = 'tmp/lua/?.lua'
require("villa_common")
require("mlist")
local json = require("cjson")
local player = hpio.player
local screen = hpio.screen
local ch_num = hpio.ch_num


local effect = {
    calssical = 1,
    modern = 2,
    rockrool = 3,
    pop = 4,
    dance = 5,
    original = 6,
}

local source = {
    ['local'] = 1,
    bluetooth = 2,
    linein = 3,
}

local volume = {
    inc = 1,
    dec = 0,
    mute = 3,
    unmute = 2,
}

local mode = {
    random = 4,
    single = 2,
    cycle = 3,
    list = 1,
}

local status = {
    [0] = 'play',
    [1] = 'pause',
    [2] = 'stop',
}

local playstate = {
    play = 1,
    pause = 0,
    prev = 2,
    next = 3,
}

local function control_cb(item)
    dump_tab(item)
    local param = item.params
    if param.volume then
        if tonumber(param.volume) then
            print("volume: ".. tostring(param.volume))
            print(math.ceil(param.volume * 8 / 3))
            player.vol(math.ceil(param.volume * 8 / 3), 0)
        else
            local v = volume[param.volume]
            if v <= 1 then
                player.vol_ctrl(volume[param.volume])
            else
                v = v - 2
                player.mute(v)
            end
        end
    end
    
    if param.mute ~= nil then
        print(bool_to_number(param.mute))
        player.mute(bool_to_number(param.mute))
    end

    if param.mode then
        player.loop(mode[param.mode])
    end
    if param.playstate then
        local p = playstate[param.playstate]
        print("playstate", p)
        if p <= 1 then
            player.play(p)
        else
            p = p - 2
            player.cut(p)
        end
    end
    if param.progress then
        player.seek(param.progress)
    end
    if param.songid then
        player.index(param.songid)
    end
    return 0, "", 0
end

local function info_cb(item)
    dump_tab(item)
    local param = item.params
    local tab = {}
    if param.getinfo then
        local info = player.status()
        tab.effect = 'original'
        local m = {}
        for k, v in pairs(mode) do
            m[v] = k
        end
        tab.mode = m[info.mode]
        tab.playstate = status[info.status]
        tab.source = 'local'
        tab.volume = math.modf((info.volume * 3 + 2) / 8)
        tab.progress = info.current_time
        -- tab.songid = info.playlist_index
        if info.playlist_index < 0 then
            tab.songid = 0
        else 
            tab.songid = info.playlist_index
        end
        local song = {}
        song.album = #info.album > 0 and info.title or nil
        song.artist = #info.author > 0 and info.title or nil
        song.duration = info.total_time
        print("song duration",song.duration)
        local test_str, test_str2 = num2char(math.floor(song.duration / 1000))
        print(test_str, test_str2)
        local test_num = ch_num.convert_force(test_str)
        print(test_num)
        local test_num = ch_num.convert_force(test_str2)
        print(test_num)
        song.title = (#info.title > 0 and info.title) or url2name(info.uri)
        -- song.title = info.title
        -- song.uri = info.uri
        song.favorite = false
        tab.currSong = song

        local info_tab = {}
        info_tab.cmd = 'info'
        info_tab.params = tab
        local str = json.encode(info_tab)
        print(str)
        return 1, str, #str
    end
    return 0,"",0
end

local function songlist(item, cmd)
    dump_tab(item)
    local str = getlist(cmd)
    return 2, str, #str
    -- local info = {}
    -- local arr = {}
    -- if str then
    --     local list = json.decode(str).list
    --     for i, v in ipairs(list) do
    --         dump_tab(v)
    --         arr[i] = {}
    --         arr[i].album = v.albumName
    --         arr[i].artist = v.authorName
    --         arr[i].duration = v.musicTime
    --         arr[i].id = v.musicId
    --         arr[i].title = v.musicName
    --         arr[i].favorite = false
    --     end
    --     info.cmd = cmd
    --     info.params = {}
    --     info.params.pageindex = 1
    --     info.params.pagesize = 0
    --     info.params.total = table.maxn(list)
    --     info.params.playlist = {}
    --     info.params.playlist.id = 0
    --     info.params.playlist.name = "default"
    --     info.params.playlist.songs = arr

    --     return 1, json.encode(info).."\n", #info
    -- end
    -- return 0, "", 0
end

local function getsonglist_cb(item)
    return songlist(item, 'getsonglist')
end

local function operlist_cb(item)
    return songlist(item, 'operlist')
end

local function exit_cb(item)
    dump_tab(item)
    print("return 9")
    return 9, nil, 0
end

local function voiceoper_cb(item)
    dump_tab(item)
    local str = item.params.content
    if str then
        return 3, str, #str
    end
    return 0, "", 0
end

local function voicespeak_cb(item)
    dump_tab(item)
    local str = item.params.content
    if str then 
        return 4, str, #str
    end
    return 0, "", 0
end

local params_path = {
    dev_path = "/mnt/udisk/device.lua",
    sce_path = "/mnt/udisk/scene.lua",
    rms_path = "/mnt/udisk/rooms.lua",
    flr_path = "/mnt/udisk/floor.lua",
}

local function save_to_file(path, str)
    local f = io.open(path, "w+")
    f:write(str.."\n")
    f:close()
end


local function getdevices_cb(item)
    local dev_arr = item.params
    for i = 1, table.maxn(dev_arr) do
        local index, name = ch_num.convert(dev_arr[i].name)
        if index >= 0 then
            dev_arr[i].name_id = name..index
        else 
            dev_arr[i].name_id = dev_arr[i].name
        end
    end
    dump_tab(item)



    save_to_file(params_path.dev_path, json.encode(dev_arr))
    -- for i, v in ipairs(dev_arr) do
    --     print(v.floor, v.floorId, v.room, v.roomId, v.id, v.name)
    -- end
    return 0, "", 0
end

local function getscenes_cb(item)
    dump_tab(item)
    -- local sce_arr = item.params 
    local i = 1
    local sce_arr = {}
    for i, v in ipairs(item.params ) do
        print(v.name)
        v.name = filter_scene(v.name)
        print(v.floor, v.floorId, v.room, v.roomId, v.id, v.name)
        sce_arr[i] = v
        i = i + 1
    end
    save_to_file(params_path.sce_path, json.encode(sce_arr))
    return 0, "", 0
end

local function getrooms_cb(item)
    dump_tab(item)
    local rms_arr = item.params
    save_to_file(params_path.rms_path, json.encode(rms_arr))
    for i, v in ipairs(rms_arr) do
        print(v.id, v.name)
    end
    return 0, "", 0
end

local function getfloors_cb(item)
    dump_tab(item)
    local flr_arr = item.params
    save_to_file(params_path.flr_path, json.encode(flr_arr))
    for i, v in ipairs(flr_arr) do
        print(v.id, v.name)
    end
    return 0, "", 0
end

local function poweroper_cb(item)
    dump_tab(item)
    local off = item.params.value
    if off and off == 1 then 
        screen.off(1)
        player.mute(1)
        player.play(0)
    else 
        screen.off(0)
        player.mute(0)
        -- player.play(1)
    end
    return 0, "", 0
end

local halo_handle = {
    control = control_cb,
    info = info_cb,
    getsonglist = getsonglist_cb,
    operlist = operlist_cb,
    exit = exit_cb,
    voiceoper = voiceoper_cb,
    voicespeak = voicespeak_cb,
    getdevices = getdevices_cb,
    getscenes = getscenes_cb,
    getrooms = getrooms_cb,
    getfloors = getfloors_cb,

    poweroper = poweroper_cb,
}

function halo_parse( str )
    print("halo parse start")
    if player.check_alarm() then
        return 0
    end
    -- print('sdfsdf',str)
    str = cut_str_to_json(str)
    local item = json.decode(str)
    if item then
        if item.cmd then
            print('cmd: '..item.cmd)
            return halo_handle[item.cmd](item)
        end
    end
    return 0
end
