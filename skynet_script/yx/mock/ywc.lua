-- ywc.lua yw客户端
local class = require "util/class"
local sc = require "socketchannel"
local P = require "protobuf"
----------------------------------------------------------------------------------------
-- 创建ywc类。
local YWClient = class("ywc");

P.register_file("d:\\yx_code\\yx\\build\\proto_client.pb");

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
	local xor = {}
	for i = 3, buf_size do
		table.insert(xor, string.char(string.byte(buf, i) ~ 165));
	end
	local v0, v1 = string.unpack(">I1I1", buf)
	if v1 & 0x3F ~= 0x02 then
		return false
	end
	return true, (v1 >> 6) << 8 | v0, table.concat(xor)
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
	return ProtoUnPack(buf, buf_size)
end

function YWClient:ctor(identId)
	print("YWClient ctor")
end

local TOSERVER_LOGIN = 2
function YWClient:connect_gateway()
	self.channel = sc.channel {host="127.0.0.1",port=3001}
	self.channel:connect(true)
end

function YWClient:login()
	--
	local c2s_login = P.encode("c2s_login", {uid=1767,world_id=1})
	--
	local response = self.channel:request(ProtoPack(TOSERVER_LOGIN, c2s_login), ProtoResponse)
	pp.print(response);
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

