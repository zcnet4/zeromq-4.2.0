//
//  protocol_defs.h
//  yworldgateway
//
//  Created by kongque on 16/3/10.
//  Copyright © 2016年 YY. All rights reserved.
//

#ifndef SERVER_GATEWAY_PROTOCOL_DEFS_H_
#define SERVER_GATEWAY_PROTOCOL_DEFS_H_

// Protocol S -> C
enum ToClientCommand {
    TOCLIENT_GAME_BREATH = 1,                           // 1
    TOCLIENT_LOGIN,                                     // 2
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
    TOCLIENT_ATTRIBUTE_UPDATE,                          // 41
    TOCLIENT_PLAYER_EXP,                                // 42
    TOCLIENT_ENCHANT_TABLE,                             // 43
    TOCLIENT_PLAYER_INFO,                               // 44
    TOCLIENT_OPEN_MAP,                                  // 45
    TOCLIENT_NEARCHAT,                                  // 46
    TOCLIENT_SHOW_TIPS,                                 // 47
    TOCLIENT_TRADE,                                     // 48
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // All the protocol command must be added before this line
    // After you add one command, please modify TOSERVER_NUM_MSG_TYPES's Value
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    TOCLIENT_NUM_MSG_TYPES = 49,
};


// Protocol C -> S
enum ToServerCommand {
    TOSERVER_GAME_BREATH = 1,                           // 1
    TOSERVER_LOGIN,                                     // 2
    TOSERVER_INIT,                                      // 3
    TOSERVER_CLIENT_READY,                              // 4
    TOSERVER_INTERACT,                                  // 5
    TOSERVER_PLAYERPOS,                                 // 6
    TOSERVER_GOTBLOCKS,                                 // 7
    TOSERVER_DELETEDBLOCKS,                             // 8
    TOSERVER_PLAYERITEM,                                // 9
    TOSERVER_INVENTORY_ACTION,                          // 10
    TOSERVER_CHAT_MESSAGE,                              // 11
    TOSERVER_TIME_OF_DAY,                               // 12
    TOSERVER_DAMAGE,                                    // 13
    TOSERVER_BREATH,                                    // 14
    TOSERVER_RESPAWN,                                   // 15
    TOSERVER_REMOVED_SOUNDS,                            // 16
    TOSERVER_LOGOUT,                                    // 17
    TOSERVER_LANDGRAVE,                                 // 18
    TOSERVER_SHOP_ACTION,                               // 19
    TOSERVER_VILLAGE,                                   // 20
    TOSERVER_REMOVENODE,                                // 21
    TOSERVER_ACHIEVE_REQUIRE,                           // 22
    TOSERVER_SWITCH_WORLD,                              // 23
    TOSERVER_SIGNTEXT,                                  // 24
    TOSERVER_TIME_SYN,                                  // 25
    TOSERVER_SKILL_CAST,                                // 26
    TOSERVER_SKILL_SING_DONE,                           // 27
    TOSERVER_TITLE,                                     // 28
    TOSERVER_REMOVE_BUFF,                               // 29
    TOSERVER_OFFLINE_MESSAGE,                           // 30
    TOSERVER_PLAYER_PITCH_YAW,                          // 31
    TOSERVER_NPCTALK_OP,                                // 32   npc对话选择
    TOSERVER_REVIVE,                                    // 33
    TOSERVER_NEARCHAT,                                  // 34
    TOSERVER_ANALYSE_BEHAVIOR,                          // 35
    TOSERVER_TRADE,                                     // 36
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // All the protocol command must be added before this line
    // After you add one command, please modify TOSERVER_NUM_MSG_TYPES's Value
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    TOSERVER_NUM_MSG_TYPES = 37,
};

enum AccessDeniedCode {
    SERVER_ACCESSDENIED_KICK = 1,  // 被服务器强制踢下线
    SERVER_ACCESSDENIED_CUSTOM_STRING,  // 自定义错误字符串
    SERVER_ACCESSDENIED_CLIENT_EXISTS,  // 客户端已在线
    SERVER_ACCESSDENIED_CREATE_ROLE_FAILED,  // 服务器创建角色失败
    SERVER_ACCESSDENIED_NAME_SIZE_INVALID,  // 角色名字长度不符合要求
    SERVER_ACCESSDENIED_CLIENT_VERSION_TOO_OLD,  // 客户端版本过旧
    SERVER_ACCESSDENIED_SERVER_MAX_PLAYERS,  // 服务器人数已达上限
};

#endif  // SERVER_GATEWAY_PROTOCOL_DEFS_H_
