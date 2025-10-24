/**
 * @file AsyncPeriodicDemo.cpp
 * @brief 1394B通信异步消息周期性收发演示程序实现文件
 * 
 * 本程序实现了CC(Controller Controller)节点以1秒周期周期性发送异步消息，
 * 以及RN(Remote Node)节点接收并处理异步消息的功能。同时包含了消息接收统计
 * 和验证逻辑，可实时显示通信状态和性能指标。
 */

#include "AsyncPeriodicDemo.h"
#include <QThread>
#include <cstdio>
#include <cstring>
#include <conio.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
    #include <termios.h>
    #include <unistd.h>
    // Linux平台下模拟Windows的_kbhit和_getch函数
    int _kbhit(void);
    int _getch(void);
#endif

/**
 * @brief AsyncPeriodicDemo构造函数
 * @param parent 父对象指针
 */
AsyncPeriodicDemo::AsyncPeriodicDemo(QObject *parent)
    : QThread(parent)
    , hCcNode(nullptr)
    , hRnNode(nullptr)
    , bCcNodeOpened(false)
    , bRnNodeOpened(false)
{
    // 初始化统计信息
    memset(&asyncStats, 0, sizeof(asyncStats));
}

/**
 * @brief AsyncPeriodicDemo析构函数
 */
AsyncPeriodicDemo::~AsyncPeriodicDemo()
{
    // 确保节点被正确关闭
    closeNodes();
}

/**
 * @brief 初始化CC节点
 * 
 * 此函数负责打开1394B通信卡的CC(Controller Controller)节点，配置通信参数，
 * 并进行必要的初始化设置，以便后续能够正常发送异步消息。初始化过程包括：
 * 打开节点设备、设置通信速率、配置发送参数、禁用STOF接收功能。
 * 
 * @return 是否成功打开并初始化CC节点
 * @retval true CC节点初始化成功
 * @retval false CC节点初始化失败
 */
bool AsyncPeriodicDemo::initCcNode() {
    printf("【初始化】打开CC节点...\n");
    
    // 打开CC节点设备，使用指定的设备号和节点号
    hCcNode = static_cast<_TNF_Node_Struct*>(Mil1394_XT_OPEN(nullptr, const_cast<TNFU32*>(&DEVICE_NO), CC_NODE_NO));
    if (hCcNode == nullptr) {
        printf("【错误】CC节点打开失败\n");
        return false;
    }
    
    // 等待2秒，确保节点硬件完全初始化
    QThread::msleep(2000);
    printf("【初始化】CC节点打开成功，等待初始化完成...\n");
    
    // 设置通信端口速率为S400(400Mbps)
    if (Mil1394_Port_Speed_Set(hCcNode, COMM_SPEED) != OK) {
        printf("【错误】设置CC节点通信速率失败\n");
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        return false;
    }
    
    // 设置消息发送速率为S400(400Mbps)
    if (Mil1394_SEND_Speed_Set(hCcNode, COMM_SPEED) != OK) {
        printf("【错误】设置CC节点发送速率失败\n");
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        return false;
    }
    
    // 设置CC节点工作模式 - 禁用STOF接收功能（因为CC节点在本应用中只负责发送STOF消息）
    if (Mil1394_RCV_STOF_ENABLE(hCcNode, ABILITY_DISABLE) != OK) {
        printf("【错误】设置CC节点工作模式失败\n");
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        return false;
    }
    
    printf("【初始化】CC节点初始化成功\n");
    return true;
}

/**
 * @brief 配置CC节点的异步消息
 * 
 * 此函数负责配置CC节点发送异步消息的各项参数，包括：
 * 1. 初始化并配置异步消息配置结构体
 * 2. 设置异步消息各负载值，其中MessageID设置为特定标识值
 * 3. 将配置应用到CC节点
 * 
 * 消息使用0x1001作为特定识别标志，便于RN节点验证消息来源和正确性。
 * 
 * @return 是否成功配置异步消息
 * @retval true 异步消息配置成功
 * @retval false 异步消息配置失败
 */
bool AsyncPeriodicDemo::configCcNodeAsync() {
    printf("【配置】设置CC节点异步消息发送参数...\n");
    
    // 初始化并配置异步消息配置结构体
    memset(&cc2RnAsyncCfg, 0, sizeof(cc2RnAsyncCfg));
    cc2RnAsyncCfg.Header1394 = TX_PACK1394HEADER(ASYNC_PAYLOAD_LEN, 1);  // 设置1394头
    cc2RnAsyncCfg.MessageID = ASYNC_MSG_ID;  // 设置消息ID
    cc2RnAsyncCfg.HeartBeatStyle = 1;        // 心跳样式：步进方式
    cc2RnAsyncCfg.HeartBeatEnable = 1;       // 启用心跳
    cc2RnAsyncCfg.HeartBeatStep = 1;         // 心跳步长
    cc2RnAsyncCfg.HeartBeatinitValue = 0;    // 心跳初始值
    cc2RnAsyncCfg.SoftVPCenable = 1;         // 启用软件VPC
    
    // 应用异步消息配置到CC节点（索引0）
    if (MSG_ASYNC_SEND_CFG(hCcNode, 0, &cc2RnAsyncCfg) != OK) {
        printf("【错误】配置CC节点异步消息失败\n");
        return false;
    }
    
    // 初始化并设置异步消息数据
    memset(&cc2RnAsyncPackage, 0, sizeof(cc2RnAsyncPackage));
    cc2RnAsyncPackage.Header1394 = TX_PACK1394HEADER(ASYNC_PAYLOAD_LEN, 1);  // 设置1394头
    cc2RnAsyncPackage.MessageID = ASYNC_MSG_ID;  // 设置消息ID
    cc2RnAsyncPackage.payloadLen = ASYNC_PAYLOAD_LEN;  // 设置有效载荷长度
    
    // 填充异步消息数据载荷
    for (int i = 0; i < ASYNC_PAYLOAD_LEN/4 && i < 502; i++) {
        cc2RnAsyncPackage.msgData[i] = 0xABCD0000 + i;  // 填充递增数据
    }
    
    // 设置异步消息发送数据
    if (MSG_ASYNC_SEND_DATA_Set(hCcNode, 0, 0, reinterpret_cast<TNFU32*>(&cc2RnAsyncPackage)) != OK) {
        printf("【错误】设置CC节点异步消息数据失败\n");
        return false;
    }
    
    printf("【配置】CC节点异步消息参数配置成功\n");
    return true;
}

/**
 * @brief 启动CC节点异步消息发送
 * 
 * 此函数负责启动CC节点的异步消息周期性发送功能，包括：
 * 1. 启用异步接收功能（虽然CC节点主要用于发送，但底层驱动需要此功能开启）
 * 2. 启动所有异步消息周期性发送
 * 3. 设置CC节点打开状态标志
 * 4. 输出启动成功信息
 * 
 * CC节点将按照配置的1秒大周期，周期性地发送异步消息到1394B总线上。
 * 
 * @return 是否成功启动异步发送
 * @retval true 异步发送启动成功
 * @retval false 异步发送启动失败
 */
bool AsyncPeriodicDemo::startCcNodeAsyncSend() {
    printf("【启动】启动CC节点异步消息发送...\n");
    
    // 启用异步消息接收，以启动消息支持功能
    if (MSG_RECV_Ctrl(hCcNode, ABILITY_ENABLE) != OK) {
        printf("【错误】启用CC节点消息接收失败\n");
        return false;
    }
    
    // 启动所有异步消息周期性发送功能
    if (MSG_ASYNC_SEND_ALLCtrl(hCcNode, ABILITY_ENABLE) != OK) {
        printf("【错误】启动CC节点异步消息发送失败\n");
        return false;
    }
    
    // 设置CC节点打开状态标志，表示异步发送已成功启动
    bCcNodeOpened = true;
    printf("【启动】CC节点异步消息发送已启动\n");
    return true;
}

/**
 * @brief 初始化RN节点
 * @return 是否成功打开RN节点
 */
/**
 * @brief 初始化RN节点
 * 
 * 此函数负责打开1394B通信卡的RN(Remote Node)节点，配置通信参数，
 * 并进行必要的初始化设置，以便后续能够正常接收异步消息。初始化过程包括：
 * 打开节点设备、设置通信速率、启用消息接收功能。
 * 函数同时包含完善的错误处理机制，确保在初始化失败时能够正确释放已分配的资源。
 * 
 * @return 是否成功打开并初始化RN节点
 * @retval true RN节点初始化成功
 * @retval false RN节点初始化失败
 */
bool AsyncPeriodicDemo::initRnNode() {
    printf("【初始化】打开RN节点...\n");
    
    // 打开RN节点设备，使用指定的设备号和节点号
    hRnNode = static_cast<_TNF_Node_Struct*>(Mil1394_XT_OPEN(nullptr, const_cast<TNFU32*>(&DEVICE_NO), RN_NODE_NO));
    if (hRnNode == nullptr) {
        printf("【错误】RN节点打开失败\n");
        return false;
    }
    
    // 等待2秒，确保节点硬件完全初始化
    QThread::msleep(2000);
    printf("【初始化】RN节点打开成功，等待初始化完成...\n");
    
    // 设置通信端口速率为S400(400Mbps)
    if (Mil1394_Port_Speed_Set(hRnNode, COMM_SPEED) != OK) {
        printf("【错误】设置RN节点通信速率失败\n");
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        return false;
    }
    
    // 设置消息发送速率为S400(400Mbps)，即使RN主要用于接收，也需要配置发送速率
    if (Mil1394_SEND_Speed_Set(hRnNode, COMM_SPEED) != OK) {
        printf("【错误】设置RN节点发送速率失败\n");
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        return false;
    }
    
    // 设置RN节点工作模式 - 禁用STOF接收功能（RN节点在本应用中不需要接收STOF）
    if (Mil1394_RCV_STOF_ENABLE(hRnNode, ABILITY_DISABLE) != OK) {
        printf("【错误】设置RN节点工作模式失败\n");
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        return false;
    }
    
    printf("【初始化】RN节点初始化成功\n");
    return true;
}

/**
 * @brief 配置RN节点的异步消息接收
 * @return 是否成功配置异步消息接收
 */
bool AsyncPeriodicDemo::configRnNodeAsync() {
    printf("【配置】设置RN节点异步消息接收参数...\n");
    
    // 配置异步消息接收参数
    // 接收控制字计算
    TNFU32 RxCtrl = RX_PACKCONTROLWORD(ASYNC_PAYLOAD_LEN);
    
    // 接收配置数组-控制字、消息ID、保留字段
    TNFU32 RnNodeRxConfig[3] = {
        RxCtrl, ASYNC_MSG_ID, 0x0000
    };
    
    // 加载异步消息接收配置参数
    if (MSG_ASYNC_LoadCfg(hRnNode,
                          0,              // 发送配置数量（RN节点不发送）
                          1,              // 接收配置数量
                          1,              // 通道号
                          nullptr,        // 发送配置数组（RN节点不发送）
                          RnNodeRxConfig) // 接收配置数组
        != OK) {
        printf("【错误】配置RN节点异步消息接收失败\n");
        return false;
    }
    
    printf("【配置】RN节点异步消息参数配置成功\n");
    return true;
}

/**
 * @brief 启动RN节点异步消息接收
 * 
 * 此函数负责启动RN节点的异步消息接收功能，包括：
 * 1. 启用消息接收功能，为接收异步消息做准备
 * 2. 设置RN节点打开状态标志
 * 3. 输出启动成功信息
 * 
 * 启动后，RN节点将能够接收来自CC节点的异步消息，并在接收到消息时
 * 触发相应的接收事件处理。
 * 
 * @return 是否成功启动异步接收
 * @retval true 异步接收启动成功
 * @retval false 异步接收启动失败
 */
bool AsyncPeriodicDemo::startRnNodeAsyncRecv() {
    printf("【启动】启动RN节点异步消息接收...\n");
    
    // 启用消息接收功能，为接收异步消息做准备
    if (MSG_RECV_Ctrl(hRnNode, ABILITY_ENABLE) != OK) {
        printf("【错误】启用RN节点消息接收失败\n");
        return false;
    }
    
    // 设置RN节点打开状态标志，表示异步接收已成功启动
    bRnNodeOpened = true;
    printf("【启动】RN节点异步消息接收已启动\n");
    return true;
}

/**
 * @brief 获取当前系统时间（秒）
 * @return 当前系统时间（秒）
 */
double AsyncPeriodicDemo::getCurrentTime() {
    #ifdef _WIN32
        // Windows平台获取时间
        LARGE_INTEGER frequency, counter;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&counter);
        return (double)counter.QuadPart / (double)frequency.QuadPart;
    #else
        // Linux/Unix平台获取时间
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec + tv.tv_usec / 1000000.0;
    #endif
}

/**
 * @brief 验证异步消息的有效性和正确性
 * 
 * 此函数对接收的异步消息进行全面验证，确保消息来自预期的发送方
 * 并且消息传输过程中没有出现严重错误。主要验证内容包括：
 * 1. 消息ID验证 - 检查MessageID字段是否为预期值0x1001
 * 2. 数据载荷验证 - 验证数据载荷是否符合递增模式
 * 3. 错误计数更新 - 当发现无效消息时，增加错误计数
 * 
 * @param asyncMsg 接收到的异步消息结构体引用，包含所有消息字段值
 * @return 消息是否通过验证
 * @retval true 消息验证通过，是来自CC节点的有效消息
 * @retval false 消息验证失败，标识无效
 */
bool AsyncPeriodicDemo::validateAsyncMessage(const _MsgAsyn& asyncMsg) {
    // 验证消息ID是否符合预期
    if (asyncMsg.MessageID != ASYNC_MSG_ID) {
        printf("【错误】异步消息ID无效: 0x%08X，期望值: 0x%08X\n", asyncMsg.MessageID, ASYNC_MSG_ID);
        // 增加错误计数，用于后续统计和质量评估
        asyncStats.errorCount++;
        return false;
    }
    
    // 验证数据载荷是否符合递增模式
    bool dataValid = true;
    for (int i = 0; i < ASYNC_PAYLOAD_LEN/4 && i < 502; i++) {
        TNFU32 expectedValue = 0xABCD0000 + i;
        if (asyncMsg.msgData[i] != expectedValue) {
            printf("【错误】异步消息数据载荷[%d]无效: 0x%08X，期望值: 0x%08X\n", 
                   i, asyncMsg.msgData[i], expectedValue);
            dataValid = false;
            // 增加错误计数
            asyncStats.errorCount++;
            break;
        }
    }
    
    return dataValid;
}

/**
 * @brief 更新异步消息统计信息
 * 
 * 此函数负责实时更新异步消息的接收统计数据，包括接收计数、
 * 通信时长、丢包率和消息速率等关键性能指标。主要功能包括：
 * 1. 初始化统计 - 在首次接收到有效消息时，记录开始时间
 * 2. 更新接收计数 - 根据消息接收状态增加相应计数器
 * 3. 计算通信时长 - 动态更新总通信时间
 * 4. 计算性能指标 - 基于通信时长和配置周期计算期望消息数、
 *    丢包率和实际消息接收速率
 * 
 * 这些统计信息对于评估1394B总线通信质量、性能和可靠性
 * 至关重要，可以实时反映异步消息传输的稳定性。
 * 
 * @param messageReceived 指示是否成功接收到有效消息
 */
void AsyncPeriodicDemo::updateAsyncStats(bool messageReceived) {
    // 如果是第一次接收到有效消息，初始化统计并记录开始时间
    if (asyncStats.totalReceived == 0 && messageReceived) {
        asyncStats.startTime = getCurrentTime();
        printf("【统计】开始统计异步消息接收情况\n");
    }
    
    // 根据消息接收状态更新相应的计数器
    if (messageReceived) {
        // 成功接收到有效消息，增加接收计数
        asyncStats.totalReceived++;
    } else {
        // 未收到消息或消息无效，增加超时计数
        asyncStats.timeoutCount++;
    }
    
    // 更新当前时间和总通信时长
    asyncStats.endTime = getCurrentTime();
    asyncStats.totalDuration = asyncStats.endTime - asyncStats.startTime;
    
    // 计算通信性能指标（仅在有有效通信时长时）
    if (asyncStats.totalDuration > 0) {
        // 计算期望接收的消息数量 = 通信时长(微秒) / 消息周期(微秒)
        asyncStats.expectedMessages = (int)(asyncStats.totalDuration * 1000000 / LARGE_PERIOD);
        
        // 计算丢包率 = (期望消息数 - 实际接收数) / 期望消息数 * 100%
        if (asyncStats.expectedMessages > 0) {
            asyncStats.packetLossRate = 100.0 * (asyncStats.expectedMessages - asyncStats.totalReceived) / asyncStats.expectedMessages;
        }
        
        // 计算实际消息接收速率 = 实际接收数 / 通信时长
        asyncStats.messageRate = asyncStats.totalReceived / asyncStats.totalDuration;
    }
}

/**
 * @brief 显示异步消息统计结果
 */
void AsyncPeriodicDemo::displayAsyncStats() {
    printf("\n=================================================================\n");
    printf("                    异步消息接收统计结果                          \n");
    printf("=================================================================\n");
    printf("总通信时长: %.2f 秒\n", asyncStats.totalDuration);
    printf("接收到的异步消息数: %d\n", asyncStats.totalReceived);
    printf("期望接收的异步消息数: %d\n", asyncStats.expectedMessages);
    printf("超时次数: %d\n", asyncStats.timeoutCount);
    printf("错误消息数: %d\n", asyncStats.errorCount);
    printf("丢包率: %.2f%%\n", asyncStats.packetLossRate);
    printf("消息接收速率: %.2f 消息/秒\n", asyncStats.messageRate);
    
    // 获取并显示详细的错误计数
    if (hRnNode != nullptr) {
        TNFU32 errCounts[ERR_TopicIDCNT + 1] = {0};
        
        // 获取所有异步消息相关的错误计数
        Mil1394_MSG_Cnt_Get(hRnNode, ERR_Asyn_HCRCCNT, &errCounts[ERR_Asyn_HCRCCNT]);
        Mil1394_MSG_Cnt_Get(hRnNode, ERR_Asyn_MsgIDCNT, &errCounts[ERR_Asyn_MsgIDCNT]);
        Mil1394_MSG_Cnt_Get(hRnNode, ERR_Asyn_DCRCCNT, &errCounts[ERR_Asyn_DCRCCNT]);
        Mil1394_MSG_Cnt_Get(hRnNode, ERR_Asyn_VPCCNT, &errCounts[ERR_Asyn_VPCCNT]);
        Mil1394_MSG_Cnt_Get(hRnNode, ERR_Asyn_SVPCCNT, &errCounts[ERR_Asyn_SVPCCNT]);
        
        printf("\n【详细错误统计】\n");
        printf("异步消息头CRC错误: %u\n", errCounts[ERR_Asyn_HCRCCNT]);
        printf("异步消息ID错误: %u\n", errCounts[ERR_Asyn_MsgIDCNT]);
        printf("异步消息数据CRC错误: %u\n", errCounts[ERR_Asyn_DCRCCNT]);
        printf("异步消息VPC错误: %u\n", errCounts[ERR_Asyn_VPCCNT]);
        printf("异步消息软件VPC错误: %u\n", errCounts[ERR_Asyn_SVPCCNT]);
    }
    
    printf("=================================================================\n\n");
}

/**
 * @brief 接收并处理异步消息
 * 
 * 此函数负责从RN节点接收异步消息，进行有效性验证，并更新统计信息。
 * 具体功能包括：
 * 1. 初始化接收结构体以确保数据清洁
 * 2. 尝试从1394B总线接收异步消息，支持超时机制
 * 3. 当成功接收到消息时，打印完整的消息内容（所有Payload字段）
 * 4. 调用validateAsyncMessage函数验证消息的有效性和正确性
 * 5. 调用updateAsyncStats函数更新异步消息接收统计信息
 * 6. 返回接收和验证的结果状态
 * 
 * @param timeoutMs 超时时间（毫秒），决定等待异步消息的最大时长
 * @return 是否成功接收到有效异步消息
 * @retval true 成功接收并验证了有效的异步消息
 * @retval false 接收失败、超时或接收到无效消息
 */
bool AsyncPeriodicDemo::receiveAsyncMessage(int timeoutMs) {
    // 初始化接收结构体，确保之前的数据不影响本次接收结果
    memset(&receivedAsyncPackage, 0, sizeof(receivedAsyncPackage));
    
    // 尝试接收异步消息，初始假设未收到有效消息
    bool messageReceived = false;
    
    // 调用API接收异步消息，使用指定的超时时间和消息ID
    if (MSG_RECV_Packet_Asyn(hRnNode, ASYNC_MSG_ID, &receivedAsyncPackage, nullptr) == OK) {
        // 成功接收到异步消息，打印消息基本信息
        printf("【接收】成功接收到异步消息：\n");
        printf("  MessageID: 0x%08X\n", receivedAsyncPackage.MessageID);
        printf("  PayloadLen: %d 字节\n", receivedAsyncPackage.payloadLen);
        
        // 验证消息有效性，包括消息ID验证和数据载荷验证
        messageReceived = validateAsyncMessage(receivedAsyncPackage);
        
        if (messageReceived) {
            // 显示部分数据载荷内容
            printf("  数据载荷前4个值: 0x%08X 0x%08X 0x%08X 0x%08X\n",
                   receivedAsyncPackage.msgData[0],
                   receivedAsyncPackage.msgData[1],
                   receivedAsyncPackage.msgData[2],
                   receivedAsyncPackage.msgData[3]);
        }
    } else {
        // 接收超时或失败，记录超时信息
        printf("【超时】异步消息接收超时\n");
    }
    
    // 更新异步消息接收统计信息，包括接收计数、错误计数和丢包率等
    updateAsyncStats(messageReceived);
    
    return messageReceived;
}

/**
 * @brief 关闭节点并释放资源
 */
void AsyncPeriodicDemo::closeNodes() {
    printf("【清理】关闭节点并释放资源...\n");
    
    // 关闭CC节点
    if (hCcNode != nullptr) {
        // 禁用所有异步消息发送
        MSG_ASYNC_SEND_ALLCtrl(hCcNode, ABILITY_DISABLE);
        // 禁用消息接收
        MSG_RECV_Ctrl(hCcNode, ABILITY_DISABLE);
        // 关闭节点设备
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        bCcNodeOpened = false;
    }
    
    // 关闭RN节点
    if (hRnNode != nullptr) {
        // 禁用消息接收
        MSG_RECV_Ctrl(hRnNode, ABILITY_DISABLE);
        // 关闭节点设备
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        bRnNodeOpened = false;
    }
    
    printf("【清理】节点已关闭，资源已释放\n");
}

/**
 * @brief 线程运行函数 - 1394B异步消息周期性通信演示程序入口
 * 
 * 线程函数实现了完整的1394B异步消息周期性通信演示流程，包括：
 * 1. 初始化RN节点和CC节点
 * 2. 配置异步消息接收和发送参数
 * 3. 启动通信功能
 * 4. 进入通信循环，处理异步消息接收
 * 5. 实时统计和显示通信质量
 * 6. 资源清理和程序退出
 * 
 * 程序运行流程：
 * - 首先初始化RN节点（接收端），确保它准备好接收消息
 * - 然后配置并启动RN节点的异步消息接收功能
 * - 接着初始化CC节点（发送端），配置1秒大周期发送异步消息
 * - 等待节点初始化完成后，进入主循环
 * - RN节点持续接收异步消息并更新统计信息
 * - 实时显示通信性能指标（接收数、超时数、错误数、丢包率）
 * - 按任意键退出程序，清理资源并显示最终统计结果
 */
void AsyncPeriodicDemo::run() {
    // 打印程序标题和配置信息
    printf("=================================================================\n");
    printf("              1394B 异步消息周期性通信演示程序                  \n");
    printf("=================================================================\n");
    printf("功能：CC节点以大周期周期性发送异步消息，RN节点接收异步消息\n");
    printf("配置：周期=%d微秒(%.2f秒)\n", LARGE_PERIOD, (float)LARGE_PERIOD/1000000);
    printf("消息ID: 0x%04X\n", ASYNC_MSG_ID);
    printf("消息载荷长度: %d字节\n", ASYNC_PAYLOAD_LEN);
    printf("=================================================================\n\n");
    
    // 初始化节点状态标志，确保初始状态正确
    bCcNodeOpened = false;
    bRnNodeOpened = false;
    
    // 使用try-catch块捕获可能出现的异常，确保程序健壮性
    try {
        // 步骤1：先初始化RN节点（接收方），确保接收方先就绪
        if (!initRnNode()) {
            throw "RN节点初始化失败";
        }
        
        // 步骤2：配置RN节点异步消息接收参数
        if (!configRnNodeAsync()) {
            throw "RN节点异步消息配置失败";
        }
        
        // 步骤3：启动RN节点异步消息接收功能，准备接收来自CC节点的消息
        if (!startRnNodeAsyncRecv()) {
            throw "RN节点异步消息接收启动失败";
        }
        
        // 步骤4：初始化CC节点（发送方），准备发送异步消息
        if (!initCcNode()) {
            throw "CC节点初始化失败";
        }
        
        // 步骤5：配置CC节点异步消息发送参数，设置1秒大周期和消息内容
        if (!configCcNodeAsync()) {
            throw "CC节点异步消息配置失败";
        }
        
        // 步骤6：启动CC节点异步消息发送功能，开始周期性发送消息
        if (!startCcNodeAsyncSend()) {
            throw "CC节点异步消息发送启动失败";
        }
        
        // 等待两个节点都成功初始化，确保通信链路完全建立
        printf("\n【等待】等待节点初始化完成...\n");
        int waitCounter = 0;
        const int MAX_WAIT_COUNT = 10;  // 最多等待10秒，避免无限等待
        
        // 等待循环，定期检查节点状态
        while (!(bCcNodeOpened && bRnNodeOpened) && waitCounter < MAX_WAIT_COUNT) {
            QThread::msleep(1000);  // 每1秒检查一次节点状态
            waitCounter++;
            printf("【等待】已等待%d秒...\n", waitCounter);
        }
        
        // 检查节点是否成功初始化，超时则报错
        if (!(bCcNodeOpened && bRnNodeOpened)) {
            throw "节点初始化超时";
        }
        
        // 初始化成功，开始通信
        printf("\n【成功】所有节点初始化完成，开始通信...\n");
        printf("=================================================================\n");
        printf("按任意键退出程序\n");
        printf("=================================================================\n\n");
        
        // 进入消息接收主循环
        bool running = true;
        
        while (running) {
            // 尝试接收异步消息，超时时间设为周期的2倍（考虑到大周期可能较大）
            // 超时参数转换为毫秒单位
            receiveAsyncMessage(LARGE_PERIOD * 2 / 1000);
            
            // 显示实时统计信息，使用\r实现行内刷新，避免屏幕滚动过快
            printf("【实时统计】接收: %d | 超时: %d | 错误: %d | 丢包率: %.2f%%\r", 
                   asyncStats.totalReceived, asyncStats.timeoutCount, 
                   asyncStats.errorCount, asyncStats.packetLossRate);
            fflush(stdout);  // 立即刷新输出缓冲区，确保实时显示
            
            // 检查是否有键盘输入，支持用户随时退出程序
            if (_kbhit()) {  // Windows平台下检查键盘输入
                char ch = _getch();
                running = false;
                printf("\n【用户】用户按下按键，准备退出程序...\n");
            }
        }
        
        // 显示最终统计结果，全面评估通信质量
        displayAsyncStats();
        
    } catch (const char* errorMsg) {
        // 捕获字符串异常，显示具体错误信息
        printf("\n【异常】%s\n", errorMsg);
    } catch (...) {
        // 捕获未知异常，确保程序不会崩溃
        printf("\n【异常】发生未知错误\n");
    }
    
    // 无论执行过程如何，都确保正确清理资源
    closeNodes();
    
    // 程序结束提示
    printf("\n=================================================================\n");
    printf("              程序已退出                                          \n");
    printf("=================================================================\n");
}

#ifdef __linux__
/**
 * @brief Linux平台下模拟Windows的_kbhit函数
 * @return 是否有按键按下
 */
int _kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

/**
 * @brief Linux平台下模拟Windows的_getch函数
 * @return 按下的字符
 */
int _getch(void) {
    struct termios oldt, newt;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}


/**
 * @file main_async_demo.cpp
 * @brief 1394B异步消息周期性通信演示程序主函数
 * 
 * 本程序是异步消息周期性通信演示的入口点，创建并运行AsyncPeriodicDemo对象。
 */

#include "AsyncPeriodicDemo.h"
#include <QCoreApplication>

/**
 * @brief 主函数
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 程序退出状态码
 */
int main(int argc, char *argv[])
{
    // 创建Qt核心应用对象
    QCoreApplication app(argc, argv);
    
    // 创建异步消息周期性通信演示对象
    AsyncPeriodicDemo asyncDemo;
    
    // 启动异步消息通信演示
    asyncDemo.start();
    
    // 等待线程执行完成
    asyncDemo.wait();
    
    // 程序正常退出
    return 0;
}

#endif
