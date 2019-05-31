#!/usr/bin/lua
package.path = 'tmp/lua/?.lua'
require("villa_common")
local json = require("cjson")
local path = "/tmp/play_list.json"
local uri_path = '/tmp/uri_list.json'

function savelist( str )
    local tab = json.decode(str)
    local list = tab.list
    local list_tab = {}
    local page = {}
    
    list_tab.total = table.maxn(list)
    local t_tmp = list_tab.total

    if t_tmp > 0 then
        for i = 1, math.modf( (t_tmp + 1) / 2 ) do
            page[i] = {}
            local song = {}
            song[1] = list[i * 2 - 1]
            if i * 2 > t_tmp then
                page[i].num = 1
            else
                page[i].num = 2
                song[2] = list[i * 2]
            end
            page[i].song = song
        end
    end
    list_tab.page = page
    io.output(path):write(json.encode(list_tab))
    -- io.output(path):write(str)
end

function getlist( cmd )
    -- return io.input(path):read("*a")
    local str = io.input(path):read("*a")
    local tab = json.decode(str)
    local ret = {}
    ret.total = tab.total
    ret.page = table.maxn(tab.page)
    ret.cmd = cmd
    return json.encode(ret)
end

function getpage( num , cmd)
    print(num, cmd)
    local str = io.input(path):read("*a")
    local tab = json.decode(str)
    if tab.page[num] then
        local info = {}
        local arr = {}
        for i, v in ipairs(tab.page[num].song) do
            dump_tab(v)
            arr[i] = {}
            arr[i].album = #v.albumName > 0 and v.albumName or '<unknown>'
            arr[i].artist = #v.authorName > 0 and v.authorName or '<unknown>'
            arr[i].duration = v.musicTime
            arr[i].id = v.musicId
            arr[i].title = v.musicName
            arr[i].favorite = false
        end
        info.cmd = cmd
        info.params = {}
        info.params.pageindex = num
        info.params.pagesize = tab.page[num].num
        info.params.total = tab.total
        info.params.playlist = {}
        info.params.playlist.id = num
        info.params.playlist.name = "default"
        info.params.playlist.songs = arr
        return json.encode(info).."\n"
    else
        return nil
    end
end

function save_uri_list( str )
    print(str)
    io.output(uri_path):write(str)
end


local function reverse_ulist( arr )
    local ret = {}
    if type(tab) == 'table' then
        for i, v in ipairs(arr) do
            ret.i = rawget(arr.i, 1)
        end
    end
    return nil
end

function get_uri_from( index )
    print(index)
    local str = io.input(uri_path):read("*a")
    print(str)
    local tab = json.decode(str)
    dump_tab(tab)
    print(tab.array[index + 1])
    return tab.array[index + 1] or ''
end