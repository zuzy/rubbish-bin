#!/usr/bin/lua
package.path = '/tmp/lua/?.lua'
require("villa_common")
require("http_common")
local json = require("cjson")

local token = 'ZGE3M2xlbGlCMEM0bk9sN1BYS0ZYRWR3cVRBVzRxU0E='
local platformId = '794687228126175232'
local uid = "5cffff5d8be1"

local function Sleep(n)
    local t0 = os.clock()
    while os.clock() - t0 <= n do end
 end

function hope_http_regist( uri, dat)
    print("http regist")
    local str, err = http_post(uri, dat)
    return str, err
end

function hope_http_tp_dev_ctrl( id, status )

    local dev_path = '/tmp/http_dev_tp.json'
    local str = ''

    if file_exists(dev_path) then
        print(dev_path..' exists!')
        str = io.input(dev_path):read("*a")
        print(#str, str)
    end

    if #str == 0 then
        return 1
    end

    local dev_tab = json.decode(str)
    dump_tab(dev_tab)
    print("------\n")
    print('id', id, status)
    dump_tab(dev_tab[id])
    local ctrl_tab = {}
    ctrl_tab.deviceId = dev_tab[id].deviceId
    ctrl_tab.hopeCata = dev_tab[id].hopeCata
    ctrl_tab.platformId = platformId
    ctrl_tab.tokenId = token
    -- ctrl_tab.cmd = 'on'
    -- ctrl_tab.electric = 20
    ctrl_tab.uid = uid
    ctrl_tab.actionAttr = "STATE"

    if status == 1 then
        ctrl_tab.cmd = 'on'
        ctrl_tab.actionValue = 'ON'
    else
        ctrl_tab.cmd = 'off'
        ctrl_tab.actionValue = 'OFF'
    end
    ctrl_tab.delay = 0
    dump_tab(ctrl_tab)
    local dat = json.encode(ctrl_tab)
    print(dat)
    local uri = 'http://192.168.2.9:8080/hopeApi/smart/exec'
    local r_str, err = http_post(uri, dat)
    print('err', err, r_str)
    return 0
end

--[[
{
    "cmd": "on",
    "delay": 0,
    "deviceId": "b044d799296b45e6a9e79e6d83438579",
    "hopeCata": 100000,
    "light": "100",
    "platformId": 794687228126175232,
    "tokenId": "ZGE3M2xlbGlCMEM0bk9sN1BYS0ZYRWR3cVRBVzRxU0E=",
    "uid": "5cffff5d8be1"
}
]]

function hope_http_get_thirdparty_scene( )
    local uri = 'http://192.168.2.9:8080/hopeApi/smart/scene'
    local dat = '{"platformId":794687228126175232,"tokenId":"R3FNSlhHeFNJME1kelljTFRUbXdpT2ZXdWNBU01NV0w="}'
    local str, err = http_post(uri, dat)
    print(err, str)
end

function hope_http_get_thirdpatry_device(  )
    local uri = 'http://192.168.2.9:8080/hopeApi/smart/device'
    local dat = '{"platformId":794687228126175232,"tokenId":"ZGE3M2xlbGlCMEM0bk9sN1BYS0ZYRWR3cVRBVzRxU0E="}'
    local str, err = http_post(uri, dat)
    local dev_tab = json.decode(str)
    local domain = dev_tab.object.domain
    local _http = require("socket.http")
    local ret_tab = {}
    for i, v in ipairs(dev_tab.object.device.list) do
        if i > 6 then break end
        ret_tab[i] = v
        os.execute('mkdir -p /mnt/udisk/dev/'..i)
        for _i, _v in ipairs(v.images) do
            local _str, _err = _http.request(domain.._v)
            print(_err, v)
            local f = io.open('/mnt/udisk/dev/'..i..'/'.._i..'.png', "w+")
            f:write(_str)
            f:close()
            f = io.open('/mnt/udisk/dev/'..i..'/'..'dev.txt', 'w+')
            f:write(v.deviceName)
            f:close()
        end
    end
    dump_tab(ret_tab)
    str = json.encode(ret_tab)
    io.open('/tmp/http_dev_tp.json', 'w+'):write(str)

end

function hope_http_get_common_device(  )
    local uri = 'http://192.168.2.9:8080/hopeApi/smart/list'
    local dat = '{"tokenId":"ZGE3M2xlbGlCMEM0bk9sN1BYS0ZYRWR3cVRBVzRxU0E="}'

    local str, err = hope_http_regist(uri, dat)
    io.open('/tmp/http_dev_common.json', 'w+'):write(str)
end


function hope_http_get_device(  )
    -- os.execute("procrank | grep halotest | awk '{print $1}' > /tmp/halo_pid")
    -- local halo_pid = io.input("/tmp/halo_pid"):read("*a")
    -- print("halo pid is "..tonumber(halo_pid))
    -- local pid = tonumber(halo_pid)
    -- os.execute("procmem "..pid.." | grep halo >> /tmp/halo_lua_mem.log")

    -- os.execute("date >> /tmp/halo_mem.log")
    -- os.execute('procrank | grep "cmdline\\|halo" >> /tmp/halo_mem.log')

    -- local uri = 'http://192.168.2.9:8080/hopeApi/smart/device'
    -- local dat = '{"platformId":794687228126175232,"tokenId":"ZGE3M2xlbGlCMEM0bk9sN1BYS0ZYRWR3cVRBVzRxU0E="}'

    local uri = 'http://192.168.2.9:8080/hopeApi/smart/list'
    local dat = '{"tokenId":"ZGE3M2xlbGlCMEM0bk9sN1BYS0ZYRWR3cVRBVzRxU0E="}'

    local str, err = hope_http_regist(uri, dat)
    io.open('/tmp/http_dev.json', 'w+'):write(str)
    print('ret code '..err)

    local tab = json.decode(str)
    -- dump_tab(tab)

    -- os.execute("date >> /tmp/halo_mem.log")
    -- os.execute('procrank | grep "cmdline\\|halo" >> /tmp/halo_mem.log')

    -- local png_uri = {'http://192.168.2.9:8080/multipart/open/image/2018/05/20180530/827664438264877056.png', 'http://192.168.2.9:8080/multipart/open/image/2018/05/20180530/827664438264877056.png'}
    -- local _http = require("socket.http")
    -- for i, v in ipairs(png_uri) do
    --     local _str, _err = _http.request(v)
    --     print(_err, v)
    --     local f = io.open('/mnt/udisk/png/demo'..i..'.png', "w+")
    --     f:write(_str)
    --     f:close()
    -- end
    hope_http_get_thirdpatry_device()
    hope_http_get_thirdparty_scene()
    -- hope_http_tp_dev_ctrl(3, 1)
    return err
end