#include "ccnodework.h"

CCnodeWork::CCnodeWork(QObject *parent)
    : QThread(parent) {
    // code
    // 初始化STOF消息相关结构体
    memset(&cc2RnStofPackage, 0, sizeof(cc2RnStofPackage));
    memset(&cc2RnStofCfg, 0, sizeof(cc2RnStofCfg));
    // 初始化Asyn消息相关结构体
    memset(&cc2RnAsynPackage, 0, sizeof(cc2RnAsynPackage));
    memset(&cc2RnAsynCfg, 0, sizeof(cc2RnAsynCfg));
}

// 初始化CC节点
bool CCnodeWork::initCcNode() {
    //emit statusMessage("【初始化】打开CC节点...");
    
    // 打开CC节点设备，使用指定的设备号和节点号
    hCcNode = static_cast<_TNF_Node_Struct*>(Mil1394_XT_OPEN(nullptr, &deviceNo, ccNodeNo));
    if (hCcNode == nullptr) {
        //emit errorMessage("【错误】CC节点打开失败");
        return false;
    }
    
    // 等待2秒，确保节点硬件完全初始化
    QThread::msleep(2000);
    //emit statusMessage("【初始化】CC节点打开成功，等待初始化完成...");
    
    // 设置通信端口速率为S400(400Mbps)
    if (Mil1394_Port_Speed_Set(hCcNode, COMM_SPEED) != OK) {
        //emit errorMessage("【错误】设置CC节点通信速率失败");
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        return false;
    }
    
    // 设置消息发送速率为S400(400Mbps)
    if (Mil1394_SEND_Speed_Set(hCcNode, COMM_SPEED) != OK) {
        //emit errorMessage("【错误】设置CC节点发送速率失败");
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        return false;
    }
    
    // 设置CC节点工作模式 - 禁用STOF接收功能（因为CC节点在本应用中只负责发送STOF消息）
    if (Mil1394_RCV_STOF_ENABLE(hCcNode, ABILITY_DISABLE) != OK) {
        //emit errorMessage("【错误】设置CC节点工作模式失败");
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        return false;
    }
    
    //emit statusMessage("【初始化】CC节点初始化成功");
    return true;
}

// 配置CC节点STOF消息
bool CCnodeWork::configCcNodeStof() {
    //emit statusMessage("【配置】设置CC节点STOF发送参数...");
    
    // 设置STOF消息发送周期为大周期(1秒)
    if (MSG_STOF_Period(hCcNode, LARGE_PERIOD) != OK) {
        //emit errorMessage("【错误】设置CC节点STOF周期失败");
        return false;
    }
    
    // 初始化并配置STOF配置结构体
    memset(&cc2RnStofCfg, 0, sizeof(cc2RnStofCfg));
    cc2RnStofCfg.STOFPeriod = LARGE_PERIOD;  // 设置大周期为1秒(1,000,000微秒)
    cc2RnStofCfg.SysCntType = 1;             // 系统计数类型，用于时间同步
    cc2RnStofCfg.STOFPayload4 = 0;           // 初始化负载4值
    // 应用STOF配置到CC节点
    if (MSG_STOF_SEND_CFG(hCcNode, &cc2RnStofCfg) != OK) {
        //emit errorMessage("【错误】配置CC节点STOF消息失败");
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
        //emit errorMessage("【错误】设置CC节点STOF消息数据失败");
        return false;
    }
    
    //emit statusMessage(QString("【配置】CC节点STOF参数配置成功，周期：%1微秒").arg(LARGE_PERIOD));
    return true;
}

// 配置CC节点Asyn消息发送
bool CCnodeWork::configCcNodeAsynSend() {
    //printf("【配置】设置CC节点异步消息发送参数...\n");
    
    // 初始化并配置异步消息配置结构体
    memset(&cc2RnAsynCfg, 0, sizeof(cc2RnAsynCfg));
    cc2RnAsynCfg.Header1394 = TX_PACK1394HEADER(CC2RN_ASYNC_PAYLOAD_LEN, 1);  // 设置1394头
    cc2RnAsynCfg.MessageID = CC2RN_ASYNC_MSG_ID;  // 设置消息ID
    cc2RnAsynCfg.HeartBeatStyle = 1;        // 心跳样式：步进方式
    cc2RnAsynCfg.HeartBeatEnable = 1;       // 启用心跳
    cc2RnAsynCfg.HeartBeatStep = 1;         // 心跳步长
    cc2RnAsynCfg.HeartBeatinitValue = 0;    // 心跳初始值
    cc2RnAsynCfg.SoftVPCenable = 1;         // 启用软件VPC
    
    // 应用异步消息配置到CC节点（索引0）
    if (MSG_ASYNC_SEND_CFG(hCcNode, 0, &cc2RnAsynCfg) != OK) {
        //printf("【错误】配置CC节点异步消息失败\n");
        return false;
    }
    
    // 初始化并设置异步消息数据
    memset(&cc2RnAsynPackage, 0, sizeof(cc2RnAsynPackage));
    cc2RnAsynPackage.Header1394 = TX_PACK1394HEADER(CC2RN_ASYNC_PAYLOAD_LEN, 1);    // 设置1394头
    cc2RnAsynPackage.MessageID = CC2RN_ASYNC_MSG_ID;                                // 设置消息ID
    cc2RnAsynPackage.payloadLen = CC2RN_ASYNC_PAYLOAD_LEN;                          // 设置有效载荷长度
    
    // 填充异步消息数据载荷
    for (int i = 0; i < CC2RN_ASYNC_PAYLOAD_LEN/4 && i < 502; i++) {
        cc2RnAsynPackage.msgData[i] = 0xABCD0000 + i;                               // 填充递增数据
    }
    
    // 设置异步消息发送数据
    if (MSG_ASYNC_SEND_DATA_Set(hCcNode, 0, 0, reinterpret_cast<TNFU32*>(&cc2RnAsynPackage)) != OK) {
        //printf("【错误】设置CC节点异步消息数据失败\n");
        return false;
    }
    
    //printf("【配置】CC节点异步消息参数配置成功\n");
    return true;
}

// 配置CC节点Asyn消息接收
bool CCnodeWork::configCcNodeAsynRecv() {
    //printf("【配置】设置CC节点异步消息接收参数...\n");
    
    // 加载异步消息接收配置参数
    if (MSG_ASYNC_LoadCfg(hCcNode,
                          1,              // 发送配置数量（RN节点不发送）
                          1,              // 接收配置数量
                          1,              // 通道号
                          CcNodeTxConfigArr,        // 发送配置数组（RN节点不发送）
                          CcNodeRxConfigArr) // 接收配置数组
        != OK) {
        //printf("【错误】配置CC节点异步消息接收失败\n");
        return false;
    }
    
    //printf("【配置】CC节点异步消息参数配置成功\n");
    return true;
}

// 启动CC节点STOF消息发送
bool CCnodeWork::startCcNodeStofSend() {
    //emit statusMessage("【启动】启动CC节点STOF消息发送...");
    
    // 启用异步消息接收，以启动消息支持功能
    if (MSG_RECV_Ctrl(hCcNode, ABILITY_ENABLE) != OK) {
        //emit errorMessage("【错误】启用CC节点消息接收失败");
        return false;
    }
    
    // 启动STOF消息周期性发送功能
    if (MSG_STOF_SEND_Ctrl(hCcNode, ABILITY_ENABLE, LARGE_PERIOD, 0, 0) != OK) {
        //emit errorMessage("【错误】启动CC节点STOF消息发送失败");
        return false;
    }
    
    // 设置CC节点打开状态标志，表示STOF发送已成功启动
    bCcNodeOpened = true;
    //emit statusMessage("【启动】CC节点STOF消息发送已启动");
    return true;
}

// 停止CC节点STOF消息发送
bool CCnodeWork::stopCcNodeStofSend() {
    //emit statusMessage("【停止】停止CC节点STOF消息发送...");
    
    // 禁用STOF消息周期性发送功能
    if (MSG_STOF_SEND_Ctrl(hCcNode, ABILITY_DISABLE, LARGE_PERIOD, 0, 0) != OK) {
        //emit errorMessage("【错误】停止CC节点STOF消息发送失败");
        return false;
    }
    
    //emit statusMessage("【停止】CC节点STOF消息发送已停止");
    return true;
}

// 启动CC节点Asyn消息发送
bool CCnodeWork::startCcNodeAsynSend() {
    //printf("【启动】启动CC节点异步消息发送...\n");
    
    // 启用异步消息接收，以启动消息支持功能
    if (MSG_RECV_Ctrl(hCcNode, ABILITY_ENABLE) != OK) {
        //printf("【错误】启用CC节点消息接收失败\n");
        return false;
    }
    
    // 启动所有异步消息周期性发送功能
    if (MSG_ASYNC_SEND_ALLCtrl(hCcNode, ABILITY_ENABLE) != OK) {
        //printf("【错误】启动CC节点异步消息发送失败\n");
        return false;
    }
    
    // 设置CC节点打开状态标志，表示异步发送已成功启动
    bCcNodeOpened = true;
    //printf("【启动】CC节点异步消息发送已启动\n");
    return true;
}

// 启动CC节点Asyn消息接收
bool CCnodeWork::startCcNodeAsynRecv() {
    // 启用消息接收功能，为接收异步消息做准备
    if (MSG_RECV_Ctrl(hCcNode, ABILITY_ENABLE) != OK) {
        //printf("【错误】启用CC节点消息接收失败\n");
        return false;
    }
    
    // 设置CC节点打开状态标志，表示异步接收已成功启动
    bCcNodeOpened = true;
    //printf("【启动】CC节点异步消息接收已启动\n");
    return true;
}

// 停止CC节点Asyn消息发送
bool CCnodeWork::stopCcNodeAsynSend() {
    return true;
}

// 停止CC节点Asyn消息接收
bool CCnodeWork::stopCcNodeAsynRecv() {
    return true;
}

// 关闭CC节点并释放资源
void CCnodeWork::closeCcNode() {
    //emit statusMessage("【清理】关闭CC节点并释放资源...");
    
    // 关闭CC节点
    if (hCcNode != nullptr) {
        // 禁用STOF消息发送
        MSG_STOF_SEND_Ctrl(hCcNode, ABILITY_DISABLE, LARGE_PERIOD, 0, 0);
        // 禁用消息接收
        MSG_RECV_Ctrl(hCcNode, ABILITY_DISABLE);
        // 关闭节点设备
        Mil1394_Close(hCcNode);
        hCcNode = nullptr;
        bCcNodeOpened = false;
    }
    
    //emit statusMessage("【清理】CC节点已关闭，资源已释放");
}

// 获取Cc节点的消息计数
void CCnodeWork::getCcMessageCounts() {
    if (hCcNode == nullptr) {
        return;
    }

    for (int i = BusResetCNT; i <= ERR_TopicIDCNT; i++) {
        Mil1394_MSG_Cnt_Get(hCcNode, i, &ccMeassageCounts[i]);
    }
}

// 处理CC节点接收到的消息
void CCnodeWork::processMessage(_RcvMsgList *msgList) {
    // 确保是一条可用的消息
    if (msgList == nullptr || msgList->msgstateAddr == nullptr) {
        return;
    }
    
    // 获取消息状态
    _MsgState* msgState = msgList->msgstateAddr;
    
    // 根据消息类型进行处理
    switch (msgState->MessageTYPE) {
        case MSG_BUSRESET:
            //std::cout << "【总线复位消息】接收到总线复位消息" << std::endl;
            break;
            
        case MSG_STOF:
            //
            //memset(&receivedStofPackage, 0, sizeof(receivedStofPackage));
            //if (MSG_RECV_Packet_STOF(hRnNode, &receivedStofPackage, msgState) == OK) {
            //    //std::cout << "  Payload0: 0x" << std::hex << stofMsg.STOFPayload0 << std::dec << std::endl;
            //    //std::cout << "  Payload3: 0x" << std::hex << stofMsg.STOFPayload3 << std::dec << std::endl;
            //}
            break;
            
        case MSG_ASYNC: 
            //
            memset(&receivedAsynPackage, 0, sizeof(receivedAsynPackage));
            if (MSG_RECV_Packet_Asyn(hCcNode, msgState->MessageID, &receivedAsynPackage, msgState) == OK) {
                //std::cout << "  MessageID: 0x" << std::hex << asyncMsg.MessageID << std::dec << std::endl;
                //std::cout << "  PayloadLen: " << asyncMsg.payloadLen << " 字节" << std::endl;
            }
            break;
            
        case MSG_EVENT:
            //std::cout << "【事件消息】接收到事件消息，ID: 0x" << std::hex << msgState->MessageID << std::dec << std::endl;
            break;
            
        default:
            //::cout << "【未知消息】接收到未知类型消息，类型: " << msgState->MessageTYPE << std::endl;
            break;
    }
}

// 轮询处理消息
void CCnodeWork::pollAndProcessMessages() {
    // 检查是否有消息待处理
    TNFU32 msgCount = 0;
    _RcvMsgList* msgList = nullptr;
    _RcvMsgList* msgLastList = nullptr;
    
    // 获取消息列表
    if (MSG_RECV_list(hCcNode, &msgCount, &msgList, &msgLastList) == OK) {
        if (msgCount > 0) {
            // 遍历消息列表并处理每条消息
            _RcvMsgList* currentMsg = msgList;
            for (TNFU32 i = 0; i < msgCount && currentMsg != nullptr; i++) {
                processMessage(currentMsg);
                currentMsg = currentMsg->next;
            }
        } 
        else {
            // 没有消息待处理
        }
    }
    else {
        // 消息列表获取失败
    }
}

// 线程运行函数
void CCnodeWork::run() {
    // 初始化CC节点（发送方），准备发送STOF消息
    if (!initCcNode()) {
        //"CC节点初始化失败"
    }
        
    // 配置CC节点STOF发送参数，设置1秒大周期和消息内容
    if (!configCcNodeStof()) {
        //"CC节点STOF配置失败"
    }

    // 配置CC节点Asyn接收参数
    if (!configCcNodeAsynRecv()) {

    }

    // 配置CC节点Asyn发送参数，设置1秒大周期和消息内容
    if (!configCcNodeAsynSend()) {

    }

    // 启动CC节点Asyn接收功能
    if (!startCcNodeAsynRecv()) {

    }

    // 启动CC节点STOF发送功能，开始周期性发送STOF消息
    if (!startCcNodeStofSend()) {
        //"CC节点STOF发送启动失败"
    }

    // 启动CC节点Asyn发送功能
    if (!startCcNodeAsynSend()) {

    }
        
    // 初始化成功，开始发送
    //emit statusMessage("【成功】CC节点初始化完成，开始周期性发送STOF消息...");
        
    // 进入消息发送主循环
    while (!bStopRequested) {
        // 线程休眠，让出CPU时间片
        //QThread::msleep(100);

        pollAndProcessMessages();

        getCcMessageCounts();
    }

    // 清理资源
    closeCcNode();
}
