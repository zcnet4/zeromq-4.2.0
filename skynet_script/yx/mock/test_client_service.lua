-- test_gf.lua 测试gateway-forward服务。
local skynet = require "skynet"
local param_mgr = require "robot/param_mgr"
local _NEW_CLINET = require "mock/client".new
----------------------------------------------------------------------------------------
local gateway_forward_host = "127.0.0.1:2020"
--每一个client的服务会最多建立多少个连接（UID区域）
local client_max_num = 500
--每一个client的服务会建立多少个连接
local client_total_num = 200
local client_list = {}
----------------------------------------------------------------------------------------
-- local MOCK = {}
-- local N = 32
-- local cs = {}
-- function MOCK.Init()
-- 	cs[N] = true
-- 	for i = 1, N do
-- 		cs[i] = skynet.newservice(SERVICE_NAME, "slave", i)
-- 	end
-- 	for k,v in ipairs(cs) do
-- 		skynet.send(v, "lua", "send_msg")
-- 	end
-- end
----------------------------------------------------------------------------------------
-- local uid, package_num = ...
local ss, service_idx = ...
local harbor_id = tonumber(skynet.getenv("harbor") or 0)
local uid = harbor_id * service_idx * client_max_num
	
local CMD = {}

local function send(client, size, num_sec, times)
    print("[".. (skynet.now() * 10) .."]send_msg ing id:" .. service_idx .. "t:" .. times)
	for i = 1, num_sec do
		client:send(size)
	end
	if times > 1 then
		skynet.timeout(100, function()
			send(client, size, num_sec, times - 1)
		end)
	end
end

function CMD.send_msg(...)
	-- package_size(一个包的大小big:大包, small:小包)
	-- package_num(一秒发多少个包)
	-- total_sec(一共多少秒)
	-- if client_total_num > #client_list then
	-- 	skynet.error("send msg err: init client table not enough")
	-- 	return
	-- end
	local package_num = param_mgr.get_packagenum();
	local package_size = param_mgr.get_packagesize();
	local total_sec = param_mgr.get_sendtime();
	print("[".. (skynet.now() * 10) .."]send_msg begin id:" .. service_idx .. " num:" .. package_num .. " size:" .. package_size.. " time:".. total_sec.. "s")
	for i = 1, client_total_num do
		local client = client_list[i]
		send(client, package_size, package_num, total_sec)
		-- for j = 1, package_num do
		-- 	client:send(package_size)
		-- end
	end
	print("[".. (skynet.now() * 10) .."]send_msg end id:" .. service_idx)
end

function CMD.connect()
	print("connect socket service_idx:" .. service_idx)
	for i = 1, client_total_num do
		local client = client_list[i]
		client:connect(gateway_forward_host)
	end
	print("connect socket end service_idx:" .. service_idx)
end

function CMD.close()
	print("close socket service_idx:" .. service_idx)
	for i = 1, client_total_num do
		local client = client_list[i]
		client:close()
	end
	print("close socket end service_idx:" .. service_idx)
end

function CMD.clear()
	print("clear data service_idx:" .. service_idx)
	for i = 1, client_total_num do
		client_list[i] = nil
	end
	client_list = {}
	print("clear data end service_idx:" .. service_idx)
end

--退出服务
function CMD.exit()
	print("test_client_service cmd.exit")
	CMD.close()
	CMD.clear()
	skynet.exit()
end

----------------------------------------------------------------------

local MAIN = {}

function MAIN.init()
	client_total_num = param_mgr.get_socketnum()
	--初始化一次把服务的所有连接对象初始化完
	print("test client init, service_idx:" .. service_idx .. ", start uid:" .. uid)
	for i = 1, client_total_num do
		client_list[i] = _NEW_CLINET(uid + i)
	end
end

----------------------------------------------------------------------

skynet.start(function()
	MAIN.init()


	-- --测试方法
	-- skynet.timeout(10, function()
	-- 	--
	-- 	--CMD.connect();
	-- 	skynet.call(skynet.self(), "lua", "connect")
	-- end)

	skynet.dispatch("lua", function (_, source, cmd, ...)
		local f = CMD[cmd]
		if f then
			f(...)
		end
	end)
end)
