//
//  gateway_gameserver_def.h
//  yworld
//
//  Created by hanhongjun on 2017/1/12.
//  Copyright 2017年 YY. All rights reserved.

#ifndef COMMON_GATEWAY_GAMESERVER_DEF_H_
#define COMMON_GATEWAY_GAMESERVER_DEF_H_

// gameserver发送到gateway的消息
#define QMT_GAME                    1   // gameserver需要发送给客户端的消息，由网关直接转发
#define QMT_GAMESERVER_START        2   // gameserver启动或重启
#define QMT_GAMESERVER_ACTIVE       3   // gameserver处于可用状态
#define QMT_DEL_PEER                4   // gameserver通知网关断开连接
#define QMT_PLAYER_LOGTOU_AND_SAVED 5   // gameserver通知网关用户已经退出并存盘成功
#define QMT_REPORT_ONLINE_NUM       6   // gameserver上报当前世界在线人数到网关
#define QMT_GAME_BREATHE            7   // gameServer的呼吸包
#define QMT_INVALID                 8   // gameserver生成的无效消息
#define QMT_GAME_BROADCAST          9   // gameserver游戏广播消息，网关向当前世界所有客户端发送此消息
#define QMT_SWITCH_WORLD            10  // gameserver将切换世界消息转发到网关，由网关执行切换世界逻辑
#define QMT_QUEUESERVER_RESULT      11 // gameserver告诉网关排队服务器通过验证


// gateway发送到gameserver的消息
#define QMT_CLIENT                  1  // gateway转发客户端发过来的消息到gameserver
#define QMT_GATEWAY_START           2 // gateway启动或重启
#define QMT_GATEWAY_ACTIVE          3 // gateway处于可用状态


#endif

