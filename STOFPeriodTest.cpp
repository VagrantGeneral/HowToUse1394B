#include "XT1394B/Mil1394_Emu_XT.h"

// 节点定义
#define CC_NODE_NO        0   // CC节点编号
#define RN_NODE_NO        1   // RN节点编号
#define DEVICE_NO         0   // 设备编号

// 通信参数定义
#define COMM_SPEED        PORTSPEED_S400    // 通信速率：400Mbps
#define PERIOD            1000000           // 周期：1秒(1,000,000微秒)
#define PERIOD_LIMIT      50000             // 容错范围：50毫秒(50,000微秒)

// STOF消息统计结构体
typedef struct {
    int totalReceived;         // 总共接收到的STOF消息数
    int expectedMessages;      // 期望接收到的STOF消息数
    int errorCount;            // 错误消息计数
    int timeoutCount;          // 超时计数
    unsigned int lastCounter;  // 上一次接收到的计数器值
    double startTime;          // 通信开始时间（秒）
    double endTime;            // 通信结束时间（秒）
    double totalDuration;      // 总通信时长（秒）
    double packetLossRate;     // 丢包率
    double messageRate;        // 消息接收速率（消息/秒）
} StofStats;

_TNF_Node_Struct* hCcNode = nullptr;        // CC节点句柄
_TNF_Node_Struct* hRnNode = nullptr;        // RN节点句柄
bool bCcNodeOpened_       = false;          // CC节点打开状态标志
bool bRnNodeOpened_       = false;          // RN节点打开状态标志
StofStats stofStats       = {0};            // STOF消息统计信息实例

_MsgSTOF cc2RnStofPackage;                  // CC到RN的STOF消息包
_TNF_STOFCFG_Struct cc2RnStofCfg;           // CC到RN的STOF配置结构体
_MsgSTOF receivedStofPackage;               // 接收的STOF消息包

// CC节点打开并初始化
bool initCcNode() {
    // 打开CC节点设备
    hCcNode = static_cast<_TNF_Node_Struct*>(Mil1394_XT_OPEN(nullptr, &DEVICE_NO, CC_NODE_NO));
    if (hCcNode == nullptr) {
        printf("【错误】CC节点打开失败\n");
        return false;
    }
    
    // 等待2秒，确保节点硬件完全初始化
    msleep(2000);
    
    // 设置通信端口速率为S400(400Mbps)
    if (Mil1394_Port_Speed_Set(hCcNode, COMM_SPEED) != OK) {
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        return false;
    }
    
    // 设置消息发送速率为S400(400Mbps)
    if (Mil1394_SEND_Speed_Set(hCcNode, COMM_SPEED) != OK) {
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        return false;
    }
    
    // 设置CC节点工作模式 - 禁用STOF接收功能 - CC节点只发送
    if (Mil1394_RCV_STOF_ENABLE(hCcNode, ABILITY_DISABLE) != OK) {
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        return false;
    }
    
    return true;
}   

// 配置CC节点STOF的消息
bool configCcNodeStof() {
    // 设置STOF消息发送周期为大周期(1秒)
    if (MSG_STOF_Period(hCcNode, PERIOD) != OK) {
        return false;
    }
    
    // 初始化并配置STOF配置结构体
    memset(&cc2RnStofCfg, 0, sizeof(cc2RnStofCfg));
    cc2RnStofCfg.STOFPeriod = LARGE_PERIOD;  // 设置周期为1秒(1,000,000微秒)
    cc2RnStofCfg.SysCntType = 1;             // 系统计数类型, 用于时间同步
    cc2RnStofCfg.STOFPayload4 = 0;           // 初始化负载4值, 为0
    // 在这里SysCntType是设置计数，如果想要用系统计数就设为1，并在负载4上设置初始值，如果为0，则表示软件手动计数，需要我们填写。
    
    // 应用STOF配置到CC节点
    if (MSG_STOF_SEND_CFG(hCcNode, &cc2RnStofCfg) != OK) {
        printf("【错误】配置CC节点STOF消息失败\n");
        return false;
    }
    
    // 初始化并设置STOF消息数据
    memset(&cc2RnStofPackage, 0, sizeof(cc2RnStofPackage));
    
    // 设置STOF消息的各个负载值
    cc2RnStofPackage.STOFPayload0 = 0xCCCC0001;  // 消息标识，用于RN节点识别CC节点消息
    cc2RnStofPackage.STOFPayload1 = 0x00000000;  // 时间戳低位，初始化为0
    cc2RnStofPackage.STOFPayload2 = 0x00000000;  // 时间戳高位，初始化为0
    cc2RnStofPackage.STOFPayload3 = 0x00000000;  // 消息计数器，初始化为0
    cc2RnStofPackage.STOFPayload4 = 0x00000000;  // 备用负载值
    cc2RnStofPackage.STOFPayload5 = 0x00000000;  // 备用负载值
    cc2RnStofPackage.STOFPayload6 = 0x00000000;  // 备用负载值
    cc2RnStofPackage.STOFPayload7 = 0x00000000;  // 备用负载值
    cc2RnStofPackage.STOFPayload8 = 0x00000000;  // 备用负载值
    
    // 设置STOF消息发送数据
    if (MSG_STOF_SEND_DATA_Set(hCcNode, &cc2RnStofPackage) != OK) {
        return false;
    }
    
    return true;
}

// 启用CC节点STOF的发送
bool startCcNodeStofSend() {
    // 启用异步消息接收，以启动消息支持功能
    if (MSG_RECV_Ctrl(hCcNode, ABILITY_ENABLE) != OK) {
        return false;
    }
    
    // 启动STOF消息周期性发送功能
    if (MSG_STOF_SEND_Ctrl(hCcNode, ABILITY_ENABLE, PERIOD, 0, 0) != OK) {
        return false;
    }
    
    // 设置CC节点打开状态标志，表示STOF发送已成功启动
    bCcNodeOpened_ = true;
    return true;
}


// RN节点打开并初始化
bool initRnNode() {
    // 打开RN节点设备
    hRnNode = static_cast<_TNF_Node_Struct*>(Mil1394_XT_OPEN(nullptr, &DEVICE_NO, RN_NODE_NO));
    if (hRnNode == nullptr) {
        return false;
    }
    
    // 等待2秒，确保节点硬件完全初始化
    msleep(2000);
    
    // 设置通信端口速率为S400(400Mbps)
    if (Mil1394_Port_Speed_Set(hRnNode, COMM_SPEED) != OK) {
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        return false;
    }
    
    // 设置消息发送速率为S400(400Mbps)
    if (Mil1394_SEND_Speed_Set(hRnNode, COMM_SPEED) != OK) {
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        return false;
    }
    
    // 设置RN节点工作模式 - 启用STOF接收功能 - RN节点需要接收STOF消息
    if (Mil1394_RCV_STOF_ENABLE(hRnNode, ABILITY_ENABLE) != OK) {
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        return false;
    }
    
    return true;
}

// 配置RN节点STOF接收消息
bool configRnNodeStof() {
    // 设置STOF消息接收周期（与CC节点保持一致）
    if (MSG_STOF_Period(hRnNode, LARGE_PERIOD) != OK) {
        return false;
    }
    
    // 设置STOF接收门限（容错范围）
    if (MSG_RECV_STOF_limitPer(hRnNode, PERIOD_LIMIT) != OK) {
        return false;
    }
    
    return true;
}

// 启用RN节点接收
bool startRnNodeStofRecv() {
    // 启用消息接收功能，以启动消息支持功能
    if (MSG_RECV_Ctrl(hRnNode, ABILITY_ENABLE) != OK) {
        return false;
    }
    
    // 设置RN节点打开状态标志，表示STOF接收已成功启动
    bRnNodeOpened_ = true;
    return true;
}


/**
 * 更新STOF消息统计信息
 * 
 * 此函数负责实时更新STOF消息的接收统计数据，包括接收计数、
 * 通信时长、丢包率和消息速率等关键性能指标。主要功能包括：
 * 1. 初始化统计 - 在首次接收到有效消息时，记录开始时间
 * 2. 更新接收计数 - 根据消息接收状态增加相应计数器
 * 3. 计算通信时长 - 动态更新总通信时间
 * 4. 计算性能指标 - 基于通信时长和配置周期计算期望消息数、
 *    丢包率和实际消息接收速率
 * 
 * 这些统计信息对于评估1394B总线通信质量、性能和可靠性
 * 至关重要，可以实时反映STOF消息传输的稳定性。
 */
void updateStofStats(bool messageReceived) {
    // 如果是第一次接收到有效消息，初始化统计并记录开始时间
    if (stofStats.totalReceived == 0 && messageReceived) {
        stofStats.startTime = getCurrentTime();
    }
    
    // 根据消息接收状态更新相应的计数器
    if (messageReceived) {
        // 成功接收到有效消息，增加接收计数
        stofStats.totalReceived++;
    } 
    else {
        // 未收到消息或消息无效，增加超时计数
        stofStats.timeoutCount++;
    }
    
    // 更新当前时间和总通信时长
    stofStats.endTime = getCurrentTime();
    stofStats.totalDuration = stofStats.endTime - stofStats.startTime;
    
    // 计算通信性能指标（仅在有有效通信时长时）
    if (stofStats.totalDuration > 0) {
        // 计算期望接收的消息数量 = 通信时长(微秒) / 消息周期(微秒)
        stofStats.expectedMessages = (int)(stofStats.totalDuration * 1000000 / LARGE_PERIOD);
        
        // 计算丢包率 = (期望消息数 - 实际接收数) / 期望消息数 * 100%
        if (stofStats.expectedMessages > 0) {
            stofStats.packetLossRate = 100.0 * (stofStats.expectedMessages - stofStats.totalReceived) / stofStats.expectedMessages;
        }
        
        // 计算实际消息接收速率 = 实际接收数 / 通信时长
        stofStats.messageRate = stofStats.totalReceived / stofStats.totalDuration;
    }
}



// 验证STOF消息的标识和连续性
bool validateStofMessage(const _MsgSTOF& stofMsg) {
    // 验证消息标识 - 检查Payload0的高16位是否符合CC节点的标识格式0xCCCC
    if ((stofMsg.STOFPayload0 & 0xFFFF0000) != 0xCCCC0000) {
        printf("【错误】STOF消息标识无效: 0x%08X\n", stofMsg.STOFPayload0);
        // 增加错误计数，用于后续统计和质量评估
        stofStats.errorCount++;
        return false;
    }
    
    // 验证计数器是否连续（在非首次接收时）
    if (stofStats.lastCounter != 0 && stofMsg.STOFPayload3 != stofStats.lastCounter + 1) {
        // 计数器不连续，可能表明存在丢包，但不影响当前消息的有效性
        printf("【警告】STOF消息计数器不连续: 期望0x%08X，实际0x%08X\n", 
               stofStats.lastCounter + 1, stofMsg.STOFPayload3);
        // 不返回false，因为可能只是丢包导致，而非消息无效
    }
    
    // 更新最后一个计数器值，为后续消息的连续性验证做准备
    stofStats.lastCounter = stofMsg.STOFPayload3;
    
    // 消息验证通过
    return true;
}

// 接收STOF消息
bool receiveStofMessage(int timeoutMs) {
    // 初始化接收结构体
    memset(&receivedStofPackage, 0, sizeof(receivedStofPackage));
    
    // 尝试接收STOF消息，初始假设未收到有效消息
    bool messageReceived = false;
    
    // 手动调用API接收STOF消息，使用指定的超时时间
    if (MSG_STOF_RECV(hRnNode, &receivedStofPackage, timeoutMs) == OK) {
        // 成功接收到STOF消息，打印所有Payload字段值用于调试和监控
        // printf("【接收】成功接收到STOF消息：\n");
        // printf("  Payload0: 0x%08X\n", receivedStofPackage.STOFPayload0);
        // printf("  Payload1: 0x%08X\n", receivedStofPackage.STOFPayload1);
        // printf("  Payload2: 0x%08X\n", receivedStofPackage.STOFPayload2);
        // printf("  Payload3: 0x%08X\n", receivedStofPackage.STOFPayload3);
        // printf("  Payload4: 0x%08X\n", receivedStofPackage.STOFPayload4);
        // printf("  Payload5: 0x%08X\n", receivedStofPackage.STOFPayload5);
        // printf("  Payload6: 0x%08X\n", receivedStofPackage.STOFPayload6);
        // printf("  Payload7: 0x%08X\n", receivedStofPackage.STOFPayload7);
        // printf("  Payload8: 0x%08X\n", receivedStofPackage.STOFPayload8);
        
        // 验证消息有效性，包括消息标识验证和计数器连续性检查
        messageReceived = validateStofMessage(receivedStofPackage);
    } else {
        // 接收超时或失败，记录超时信息
        printf("【超时】STOF消息接收超时\n");
    }
    
    // 更新STOF消息接收统计信息，包括接收计数、错误计数和丢包率等
    updateStofStats(messageReceived);
    
    return messageReceived;
}


//关闭节点并释放资源
void closeNodes() {
    //关闭节点
    
    // 关闭CC节点
    if (hCcNode != nullptr) {
        // 禁用STOF消息发送
        MSG_STOF_SEND_Ctrl(hCcNode, ABILITY_DISABLE, LARGE_PERIOD, 0, 0);
        // 禁用消息接收
        MSG_RECV_Ctrl(hCcNode, ABILITY_DISABLE);
        // 关闭节点设备
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        bCcNodeOpened_ = false;
    }
    
    // 关闭RN节点
    if (hRnNode != nullptr) {
        // 禁用消息接收
        MSG_RECV_Ctrl(hRnNode, ABILITY_DISABLE);
        // 关闭节点设备
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        bRnNodeOpened_ = false;
    }

    // 完毕
}

int main() {
    // 初始化节点状态标志，确保初始状态正确
    bCcNodeOpened_ = false;
    bRnNodeOpened_ = false;
    
    //
    try {
        // 步骤1：先初始化RN节点（接收方），确保接收方先就绪
        if (!initRnNode()) {
            throw "RN节点初始化失败";
        }
        
        // 步骤2：配置RN节点STOF接收参数，设置接收周期和容错范围
        if (!configRnNodeStof()) {
            throw "RN节点STOF配置失败";
        }
        
        // 步骤3：启动RN节点STOF接收功能，准备接收来自CC节点的消息
        if (!startRnNodeStofRecv()) {
            throw "RN节点STOF接收启动失败";
        }
        
        // 步骤4：初始化CC节点（发送方），准备发送STOF消息
        if (!initCcNode()) {
            throw "CC节点初始化失败";
        }
        
        // 步骤5：配置CC节点STOF发送参数，设置1秒大周期和消息内容
        if (!configCcNodeStof()) {
            throw "CC节点STOF配置失败";
        }
        
        // 步骤6：启动CC节点STOF发送功能，开始周期性发送消息
        if (!startCcNodeStofSend()) {
            throw "CC节点STOF发送启动失败";
        }
        
        // 等待两个节点都成功初始化，确保通信链路完全建立
        int waitCounter = 0;
        const int MAX_WAIT_COUNT = 10;  // 最多等待10秒，避免无限等待
        
        // 等待循环，定期检查节点状态
        while (!(bCcNodeOpened_ && bRnNodeOpened_) && waitCounter < MAX_WAIT_COUNT) {
            msleep(1000);  // 每1秒检查一次节点状态
            waitCounter++;
        }
        
        // 检查节点是否成功初始化，超时则报错
        if (!(bCcNodeOpened_ && bRnNodeOpened_)) {
            throw "节点初始化超时";
        }
        

        // 进入消息接收主循环 - - - - - - - - - - - - - - - - - - - - - - - - -
        bool running = true;
        
        while (running) {
            // 尝试接收STOF消息，超时时间设为周期的2倍（考虑到大周期可能较大）
            // 超时参数转换为毫秒单位
            receiveStofMessage(LARGE_PERIOD * 2 / 1000);
        }
        
        // 显示最终统计结果，全面评估通信质量
        
    } catch (const char* errorMsg) {
        // 捕获字符串异常，显示具体错误信息
        printf("\n【异常】%s\n", errorMsg);
    } catch (...) {
        // 捕获未知异常，确保程序不会崩溃
        printf("\n【异常】发生未知错误\n");
    }
    
    // 无论执行过程如何，都确保正确清理资源
    closeNodes();
    
    return 0;
}