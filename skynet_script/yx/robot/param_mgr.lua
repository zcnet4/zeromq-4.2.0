--参数管理
---------------------------------------------------------
local skynet = require "skynet"
local dc = require "datacenter"
local urllib = require "http.url"

--默认参数值，和有哪些参数名
local param_table = {
	sernum = 50,         --服务的数量
	socketnum = 20,      --每个服务的SOCKET数量
	packagenum = 5,     --(每秒多少个包)
	packagesize = 100,  --(单位：B/字节)
	sendtime = 60       --(发送多少秒)
}

local M = {}

--初始化数值，放进DC
function M.init_param()
    -- skynet.send("/robot/main", "lua", "AddRobot", 1234)
    for k, v in pairs(param_table) do
		skynet.send("/public/datacenter", "lua", "UPDATE", k, v)
        -- skynet.send("/public/datacenter", "lua", "UPDATE", "asefd", "asdf")
        -- skynet.send("/robot/main", "lua", "AddRobot", 1234)
        -- print("/public/datacenter ".. k .." ".. v)
    end
end


--设置数值
function M.set_param(query)
    -- print("@@@@@@@@!!!!!1 set_param ".. query)
    local q = urllib.parse_query(query)
    for k, v in pairs(q) do
        if param_table[k] then
			skynet.send("/public/datacenter", "lua", "UPDATE", k, tonumber(v))
		end
    end
end

--获取 服务的数量
function M.get_sernum()
    return dc.get("sernum")
end

--获取 每个服务的SOCKET数量
function M.get_socketnum()
    return dc.get("socketnum")
end

--获取 每秒多少个包
function M.get_packagenum()
    return dc.get("packagenum")
end

--获取 每个包的大小 单位：B/字节
function M.get_packagesize()
    return dc.get("packagesize")
end

--获取 发送多少秒
function M.get_sendtime()
    return dc.get("sendtime")
end

--获取所有参数的描述
function M.get_all_param()
    local msg = "<head><meta charset=\"utf-8\"/></head>\n"
    msg = msg .. "服务的数量(sernum): " .. dc.get("sernum") .. "</br>\n"
    msg = msg .. "每个服务的SOCKET数量(socketnum): " .. dc.get("socketnum") .. "</br>\n"
    msg = msg .. "每秒多少个包(packagenum): " .. dc.get("packagenum") .. "</br>\n"
    msg = msg .. "每个包的大小(packagesize): " .. dc.get("packagesize") .. "</br>\n"
    msg = msg .. "发送多少秒(sendtime): " .. dc.get("sendtime") .. "</br>\n"
    return msg
end

-----------
return M
