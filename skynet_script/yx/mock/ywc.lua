-- ywc.lua yw客户端
local class = require "util/class"
local sc = require "socketchannel"
local P = require "protobuf"
----------------------------------------------------------------------------------------
-- 创建ywc类。
local YWClient = class("ywc");

P.register_file("D:\\yx_code\\yx\\gateway\\conf\\proto_client.pb");
P.register_file("D:\\yx_code\\yx\\gateway\\conf\\proto_server.pb");

local function NetPack(content)
	local content_size = string.len(content)
	return string.pack(">I2c"..content_size, content_size, content)
end

local function ProtoPack(type, msg_data)
	local content_size = #msg_data
	local xor = {}
	for i = 1, content_size do
		table.insert(xor, string.char(string.byte(msg_data, i) ~ 165));
	end
	local content = table.concat(xor)
	return string.pack(">I2I1I1c"..content_size, content_size + 4, type & 0xFF, (type >> 8) << 6 | 0x2, content)
end

local function ProtoUnPack(buf, buf_size)
	local v0, v1 = string.unpack(">I1I1", buf)
	if v1 & 0x3F ~= 0x02 then
		return false
	end
	--
	local xor = {}
	for i = 3, buf_size do
		table.insert(xor, string.char(string.byte(buf, i) ~ 165));
	end
	--
	return true, { cmd = (v1 >> 6) << 8 | v0, data = table.concat(xor)}
end

local function ProtoResponse(sock)
	local sz = sock:read(2)
	sz = string.unpack(">I2", sz)
	if sz < 4 then
		return false
	end
	local buf_size = sz - 2
	print("read -- sz:", buf_size)
	local buf = sock:read(buf_size)
	print("read end")
	return ProtoUnPack(buf, buf_size)
end

function YWClient:ctor(identId)
	print("YWClient ctor")
end


function YWClient:connect_gateway()
	self.channel = sc.channel {host="127.0.0.1",port=3001}
	self.channel:connect(true)
end

local TOSERVER_QUEUE_SERVER_VALIDATE= 1 + 34
local TOSERVER_LOGIN = 2
local TOCLIENT_CAN_SEND_LOGIN= 1 + 49
function YWClient:login(worldid, uid)
	-- 先发排队消息。
	print("logining - 1")
	local c2s_queue = P.encode("c2s_queue_server_validate", {id=uid, session=0, worldid=worldid})
	local resp = self.channel:request(ProtoPack(TOSERVER_QUEUE_SERVER_VALIDATE, c2s_queue), ProtoResponse)
	print("logining - 2")
	if resp.cmd == TOCLIENT_CAN_SEND_LOGIN then
		local can_login = P.decode("s2c_client_can_send_login", resp.data, #(resp.data))
		if not can_login.worldid or not can_login.login_session or not can_login.switch_world_session then
			return false
		end
		if can_login.worldid ~= worldid then
			print("can_login.world_id:"..can_login.worldid.." require worldid:"..worldid)
			return false
		end
		print("logining - 3")
		local c2s_login = P.encode("c2s_login", {uid=uid, world_id=can_login.worldid, login_session=can_login.login_session})
		--
		local resp1 = self.channel:request(ProtoPack(TOSERVER_LOGIN, c2s_login), ProtoResponse)
		print(resp1.cmd)
	else
		print("TOSERVER_QUEUE_SERVER_VALIDATE response cmd:", cmd)
	end	
end

function YWClient:send(content)
	local response = self.channel:request(ProtoPack(3, content), ProtoResponse);
	pp.print(response);
end

function YWClient:sendtest(content)
	self.channel:request(NetPack(content));
end

----------------------------------------------------------------------------------------
return YWClient

