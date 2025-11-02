#ifndef MESSAGESTATS_H
#define MESSAGESTATS_H

#include "Mil1394_Emu_XT.h"
#include <QtGlobal>

// 发送消息-1394头
#define TX_PACK1394HEADER(payloadLength, channel) ((quint32)(payloadLength + 8 * 4) << 16) | ((quint32)channel << 8) | 0xA0
// 接收消息-接收控制字
#define RX_PACKCONTROLWORD(payloadLength) (quint32)(payloadLength + 8 * 4) / (quint32)4

// 端口速率
enum PortSpeed {
    PORTSPEED_S100 = 0,         // 100Mbps
    PORTSPEED_S200 = 1,         // 200Mbps
    PORTSPEED_S400 = 2          // 400Mbps
};

// 使能/失能
enum Ability {
    ABILITY_DISABLE = 0,        // 禁用功能
    ABILITY_ENABLE = 1          // 启用功能
};

// 节点定义
static const int RN_NODE_NO = 1;                        // RN节点编号
static const int CC_NODE_NO = 0;                        // CC节点编号
static const int DEVICE_NO  = 0;                        // 设备编号

// 通信参数定义
static const int COMM_SPEED     = PORTSPEED_S400;       // 通信速率：400Mbps
static const int LARGE_PERIOD   = 1000000;              // 大周期：1秒(1,000,000微秒)
static const int PERIOD_LIMIT   = 50000;                // 容错范围：50毫秒(50,000微秒)
static const int CC2RN_ASYNC_MSG_ID   = 0x1001;         // 异步消息ID
static const int CC2RN_ASYNC_PAYLOAD_LEN = 256;         // 异步消息载荷长度
static const int RN2CC_ASYNC_MSG_ID   = 0x2001;         // 异步消息ID
static const int RN2CC_ASYNC_PAYLOAD_LEN = 128;         // 异步消息载荷长度

// typedef struct {
//     int totalReceived;         // 总共接收到的STOF消息数
//     int expectedMessages;      // 期望接收到的STOF消息数
//     int errorCount;            // 错误消息计数
//     int timeoutCount;          // 超时计数
//     unsigned int lastCounter;  // 上一次接收到的计数器值
//     double startTime;          // 通信开始时间（秒）
//     double endTime;            // 通信结束时间（秒）
//     double totalDuration;      // 总通信时长（秒）
//     double packetLossRate;     // 丢包率
//     double messageRate;        // 消息接收速率（消息/秒）
// } StofMessageStats;

// typedef struct {
//     int totalReceived;         // 总共接收到的异步消息数
//     int expectedMessages;      // 期望接收到的异步消息数
//     int errorCount;            // 错误消息计数
//     int timeoutCount;          // 超时计数
//     unsigned int lastCounter;  // 上一次接收到的计数器值
//     double startTime;          // 通信开始时间（秒）
//     double endTime;            // 通信结束时间（秒）
//     double totalDuration;      // 总通信时长（秒）
//     double packetLossRate;     // 丢包率
//     double messageRate;        // 消息接收速率（消息/秒）
// } AsyncMessageStats;

// 消息类型枚举
enum MessageType {
    MSG_BUSRESET = 0,   // 总线复位消息
    MSG_STOF = 1,       // STOF消息
    MSG_ASYNC = 2,      // 异步流消息
    MSG_EVENT = 3       // 事件消息
};

// CC节点Asyn消息配置数组(发1收1)
TNFU32 CcNodeTxConfigArr[5] = {
    //-控制字、1394头、消息ID、保留字段
    0x0000, TX_PACK1394HEADER(CC2RN_ASYNC_PAYLOAD_LEN, 1), CC2RN_ASYNC_MSG_ID, 0x0000,
    0x8000
};

TNFU32 CcNodeRxConfigArr[3] = {
    //-控制字、消息ID、保留字段
    RX_PACKCONTROLWORD(RN2CC_ASYNC_PAYLOAD_LEN), RN2CC_ASYNC_MSG_ID, 0x0000
};

// RN节点Asyn消息配置数组(发1收1)
TNFU32 RnNodeTxConfigArr[5] = {
    //-控制字、1394头、消息ID、保留字段
    0x0000, TX_PACK1394HEADER(RN2CC_ASYNC_PAYLOAD_LEN, 1), RN2CC_ASYNC_MSG_ID, 0x0000,
    0x8000
};

TNFU32 RnNodeRxConfigArr[3] = {
    //-控制字、消息ID、保留字段
    RX_PACKCONTROLWORD(CC2RN_ASYNC_PAYLOAD_LEN), CC2RN_ASYNC_MSG_ID, 0x0000
};

// 


#endif // MESSAGESTATS_H
