#!/bin/env lua
-- module(cjson)

local json = require "cjson"

-- --hello world lua program
-- print("hello world")

-- --the first program in every language

-- io.write("hello world, from", _VERSION, "!\n")

-- str = "hello world"
-- print(str)

-- if str == "hello world" then
--     print "ok"
-- end

-- print ("this a ".."string.")
-- --[[i
-- /*

-- yunsuani
-- */
-- ]]

-- a = {1, 2}
-- b = a
-- print(a==b, a~=b) --true, false

-- a = {1, 2}
-- b = {1, 2}

-- print(a == b, a ~= b) -- false, true

-- function f(x)
--     return x * x * x - x - 1
-- end

-- print(f(5))



-- -- t ={

-- --     Age = 27
    
-- --     add = function(self, n)
    
-- --     self.Age = self.Age+n
    
-- --     end
    
-- --     }
    
-- --     print(t.Age) -- 27
    
-- --     t.add(t, 10)
    
-- --     print(t.Age) -- 37

-- t = {
--     Age = 27,
--     add = function(self, n)
--         self.Age = self.Age + n
--     end
-- }
-- print(t.Age)
-- t.add(t, 10)
-- print(t.Age)

function add(...)
    local s = 0
    for i, v in ipairs{...} do
        s = s + v
    end
    return s
end

blue = {r = 0, g = 0, b = 1}
background = blue

function des( des, a )
    local ret = des - a
    return ret
end

-- local ssjson = json.encode({
--     foo = "bar",
--     some_object = {},
--     some_array = json.empty_array
-- })

ssssss = [[{"age":"23",
                    "testArray":{"array":[8,9,11,14,25]},"Himi":"himigame.com"}]]

-- str = [[{aaa:123}]]


cjson_safe = require "cjson.safe"

function parse(str, key)
    local ret = json.decode(ssssss)
    return ret["age"]
    -- return string.len(str) + string.len( key )

end




print(parse(ssssss, "age"))



-- -- local util = require "cjson.util"

-- local function json_encode_output_type(value)
--     local text = json.encode(value)
--     print(text)
--     local val = json.decode(text)
--     print(val)
--     if string.match(text, "{.*}") then
--         return "object"
--     elseif string.match(text, "%[.*%]") then
--         return "array"
--     else
--         return "scalar"
--     end
-- end

-- print(json_encode_output_type("{aaa:123}"))

-- -- function test_decode_cycle(filename)
-- --     local obj1 = json.decode(util.file_load(filename))
-- --     local obj2 = json.decode(json.encode(obj1))
-- --     return util.compare_values(obj1, obj2)
-- -- end


-- local cjson_tests = {
--     -- Test API variables
--     { "Check module name, version",
--       function () return json._NAME, json._VERSION end, { },
--       true, { "cjson", "2.1devel" } },

--     -- Test decoding simple types
--     { "Decode string",
--       json.decode, { '"test string"' }, true, { "test string" } },
--     { "Decode numbers",
--       json.decode, { '[ 0.0, -5e3, -1, 0.3e-3, 1023.2, 0e10 ]' },
--       true, { { 0.0, -5000, -1, 0.0003, 1023.2, 0 } } },
--     { "Decode null",
--       json.decode, { 'null' }, true, { json.null } },
--     { "Decode true",
--       json.decode, { 'true' }, true, { true } },
--     { "Decode false",
--       json.decode, { 'false' }, true, { false } },
--     { "Decode object with numeric keys",
--       json.decode, { '{ "1": "one", "3": "three" }' },
--       true, { { ["1"] = "one", ["3"] = "three" } } },
--     { "Decode object with string keys",
--       json.decode, { '{ "a": "a", "b": "b" }' },
--       true, { { a = "a", b = "b" } } },
--     { "Decode array",
--       json.decode, { '[ "one", null, "three" ]' },
--       true, { { "one", json.null, "three" } } },
-- }


-- print(("==> Testing Lua CJSON version %s\n"):format(json._VERSION))




-- print(ssjson)
-- local sss = json.decode(ssssss)
-- print(sss["age"])


