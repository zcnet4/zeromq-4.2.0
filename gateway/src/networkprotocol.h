/*
Minetest
Copyright (C) 2010-2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef NETWORKPROTOCOL_HEADER
#define NETWORKPROTOCOL_HEADER

#define LATEST_PROTOCOL_VERSION 43

// Server's supported network protocol range
#define SERVER_PROTOCOL_VERSION_MIN 13
#define SERVER_PROTOCOL_VERSION_MAX LATEST_PROTOCOL_VERSION

// Client's supported network protocol range
#define CLIENT_PROTOCOL_VERSION_MIN 13
#define CLIENT_PROTOCOL_VERSION_MAX LATEST_PROTOCOL_VERSION

#define TEXTURENAME_ALLOWED_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-"

// Protocol S -> C

enum ToClientCommand
{
    TOCLIENT_GAME_BREATH = 1,                           // 1
    TOCLIENT_SERVER_READY,                              // 2
    TOCLIENT_ADDNODE,                                   // 3
    TOCLIENT_REMOVENODE,                                // 4
    TOCLIENT_TIME_OF_DAY,                               // 5
    TOCLIENT_UPDATE_TIME_OF_DAY,                        // 6
    TOCLIENT_BLOCKDATA,                                 // 7
    TOCLIENT_HP,                                        // 8
    TOCLIENT_BREATH,                                    // 9
    TOCLIENT_ACCESS_DENIED,                             // 10
    TOCLIENT_DEATHSCREEN,                               // 11
    TOCLIENT_INVENTORY,                                 // 12
    TOCLIENT_DETACHED_INVENTORY,                        // 13
    TOCLIENT_CHAT_MESSAGE,                              // 14
    TOCLIENT_MOVE_PLAYER,                               // 15
    TOCLIENT_ACTIVE_OBJECT_REMOVE_ADD,                  // 16
    TOCLIENT_ACTIVE_OBJECT_MESSAGES,                    // 17
    TOCLIENT_PLAY_SOUND,                                // 18
    TOCLIENT_STOP_SOUND,                                // 19
    TOCLIENT_PRIVILEGES,                                // 20
    TOCLIENT_BLOCKDATA_BATCH,                           // 21
    TOCLIENT_HUNGER,                                    // 22
    TOCLIENT_ON_ITEM_USE,                               // 23
    TOCLIENT_LANDGRAVE_OPERATION_RESULT,                // 24
    TOCLIENT_SHOP_ACTION_RESULT,                        // 25
    TOCLIENT_NPCTALK,                                   // 26
    TOCLIENT_ACHIEVEACK,                                // 27
    TOCLIENT_SWITCH_WORLD_ACK,                          // 28
    TOCLIENT_TIME_SYN,                                  // 29
    TOCLIENT_BUFF_ADD,                                  // 30
    TOCLIENT_BUFF_END,                                  // 31
    TOCLIENT_SKILL_CAST,                                // 32
    TOCLIENT_SKILL_OVER,                                // 33
    TOCLIENT_SKILL_SING_CANCEL,                         // 34
    TOCLIENT_SKILL_SING_OVER,                           // 35
    TOCLIENT_TITLES_LOAD,                               // 36
    TOCLIENT_SKILL_UPDATE,                              // 37
    TOCLIENT_CURRENT_TITLE,                             // 38
	TOCLIENT_BUFF_LIST,                                 // 39
    TOCLIENT_VILLAGE,                                   // 40
	TOCLIENT_ATTRIBUTE_UPDATE,							// 41
    TOCLIENT_PLAYER_EXP,                                // 42
	TOCLIENT_ENCHANT_TABLE,                             // 43
    TOCLIENT_PLAYER_INFO,                               // 44
    TOCLIENT_OPEN_MAP,                                  // 45
    TOCLIENT_NEARCHAT,                                  // 46
    TOCLIENT_SHOW_TIPS,                                 // 47
    TOCLIENT_TRADE,                                     // 48
    TOCLIENT_FURNACE_STATUS_ACK,                        // 49
    TOCLIENT_CAN_SEND_LOGIN,                            // 50
    TOCLIENT_TELEPORT,                                  // 51
    TOCLIENT_MAPNODE_INVENTORY,                         // 52
    TOCLIENT_UPDATE_WEATHER,                            // 53
    TOCLIENT_BOOKTAB_STATUS_ACK,                        // 54
    TOCLIENT_EXT_FIELDS_UPDATE,                         // 55
    TOCLIENT_WEB_POS_EDIT,                              // 56
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // All the protocol command must be added before this line
    // After you add one command, please modify TOSERVER_NUM_MSG_TYPES's Value
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    TOCLIENT_NUM_MSG_TYPES = 57,
};

// Protocol C -> S

enum ToServerCommand
{
    TOSERVER_GAME_BREATH = 1,                           // 1
    TOSERVER_LOGIN,                                     // 2
    TOSERVER_CLIENT_READY,                              // 3
    TOSERVER_INTERACT,                                  // 4
    TOSERVER_PLAYERPOS,                                 // 5
    TOSERVER_GOTBLOCKS,                                 // 6
    TOSERVER_DELETEDBLOCKS,                             // 7
    TOSERVER_PLAYERITEM,                                // 8
    TOSERVER_INVENTORY_ACTION,                          // 9
    TOSERVER_CHAT_MESSAGE,                              // 10
    TOSERVER_TIME_OF_DAY,                               // 11
    TOSERVER_DAMAGE,                                    // 12
    TOSERVER_BREATH,                                    // 13
    TOSERVER_RESPAWN,                                   // 14
    TOSERVER_LOGOUT,                                    // 15
    TOSERVER_LANDGRAVE,                                 // 16
    TOSERVER_SHOP_ACTION,                               // 17
    TOSERVER_VILLAGE,                                   // 18
    TOSERVER_REMOVENODE,                                // 19
    TOSERVER_ACHIEVE_REQUIRE,                           // 20
    TOSERVER_SIGNTEXT,                                  // 21
    TOSERVER_TIME_SYN,                                  // 22
    TOSERVER_SKILL_CAST,                                // 23
    TOSERVER_SKILL_SING_DONE,                           // 24
    TOSERVER_TITLE,                                     // 25
    TOSERVER_REMOVE_BUFF,                               // 26
    TOSERVER_OFFLINE_MESSAGE,                           // 27
    TOSERVER_PLAYER_PITCH_YAW,                          // 28
    TOSERVER_NPCTALK_OP,                                // 29   NPC对话选择
    TOSERVER_REVIVE,                                    // 30
    TOSERVER_NEARCHAT,                                  // 31
    TOSERVER_ANALYSE_BEHAVIOR,                          // 32
    TOSERVER_TRADE,                                     // 33
    TOSERVER_GET_FURNACE_STATUS,                        // 34
    TOSERVER_QUEUE_SERVER_VALIDATE,                     // 35
    TOSERVER_TELEPORT,                                  // 36   本地图传送
    TOSERVER_UPDATE_NODE_STYLE,                         // 37   更改node样式
    TOSERVER_MAPNODE_INVENTORY,                         // 38
    TOSERVER_BOOKTABOPEN_STATUS,                        // 39 传送写作台的状态
    TOSERVER_GET_ACHIEVEREWARD,                         // 40 获取成就奖励
    TOSERVER_SWITCH_WORLD_CHECK,                        // 41 切换世界的监测
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // All the protocol command must be added before this line
    // After you add one command, please modify TOSERVER_NUM_MSG_TYPES's Value
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    TOSERVER_NUM_MSG_TYPES = 42,
};

enum AccessDeniedCode {
    SERVER_ACCESSDENIED_KICK = 1, // 被服务器强制踢下线
    SERVER_ACCESSDENIED_CUSTOM_STRING, // 自定义错误字符串
    SERVER_ACCESSDENIED_CLIENT_EXISTS, // 客户端已在线
    SERVER_ACCESSDENIED_INVALID_OPERATION, // 非法操作
    SERVER_ACCESSDENIED_CREATE_ROLE_FAILED, // 服务器创建角色失败
    SERVER_ACCESSDENIED_NAME_SIZE_INVALID, // 角色名字长度不符合要求
    SERVER_ACCESSDENIED_CLIENT_VERSION_TOO_OLD, // 客户端协议版本过旧
    SERVER_ACCESSDENIED_MAP_VERSION_TOO_OLD, // 客户端地图版本过旧
    SERVER_ACCESSDENIED_SERVER_MAX_PLAYERS, // 服务器人数已达上限
    SERVER_ACCESSDENIED_CHEAT_SPEED, // 客户端使用加速器
    SERVER_ACCESSDENIED_SESSION_ERROR, // session错误
    SERVER_ACCESSDENIED_QUEUE_SERVER_VALIDATE_FAILED, // 排队服务器验证失败
    SERVER_ACCESSDENIED_QUEUE_SERVER_VALIDATE_TIMEOUT, // 排队服务器验证超时
    SERVER_ACCESSDENIED_GAMESERVER_INACTIVE, // 游戏服务器不可用
    SERVER_ACCESSDENIED_INTO_WORLD_CONNITION,   //  进入世界条件不满足
};

enum NetProtoCompressionMode {
	NETPROTO_COMPRESSION_ZLIB = 0,
};
#endif
