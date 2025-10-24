#include "rnnodework.h"

RNnodeWork::RNnodeWork(QObject *parent)
    : QThread(parent) {
    // code
    
    memset(&stofstats, 0, sizeof(stofstats));
    memset(&receivedStofPackage, 0, sizeof(receivedStofPackage));
}

// 初始化RN节点
bool RNnodeWork::initRnNode() {
    //emit statusMessage("【初始化】打开RN节点...");
    
    // 打开RN节点设备，使用指定的设备号和节点号
    hRnNode = static_cast<_TNF_Node_Struct*>(Mil1394_XT_OPEN(nullptr, const_cast<TNFU32*>(&DEVICE_NO), RN_NODE_NO));
    if (hRnNode == nullptr) {
        //emit errorMessage("【错误】RN节点打开失败");
        return false;
    }
    
    // 等待2秒，确保节点硬件完全初始化
    QThread::msleep(2000);
    //emit statusMessage("【初始化】RN节点打开成功，等待初始化完成...");
    
    // 设置通信端口速率为S400(400Mbps)
    if (Mil1394_Port_Speed_Set(hRnNode, COMM_SPEED) != OK) {
        //emit errorMessage("【错误】设置RN节点通信速率失败");
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        return false;
    }
    
    // 设置消息发送速率为S400(400Mbps)，即使RN主要用于接收，也需要配置发送速率
    if (Mil1394_SEND_Speed_Set(hRnNode, COMM_SPEED) != OK) {
        //emit errorMessage("【错误】设置RN节点发送速率失败");
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        return false;
    }
    
    // 设置RN节点工作模式 - 启用STOF接收功能（RN节点需要接收STOF消息）
    if (Mil1394_RCV_STOF_ENABLE(hRnNode, ABILITY_ENABLE) != OK) {
        //emit errorMessage("【错误】设置RN节点工作模式失败");
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        return false;
    }
    
    //emit statusMessage("【初始化】RN节点初始化成功");
    return true;
}

// 配置RN节点的STOF接收参数
bool RNnodeWork::configRnNodeStof() {
    //emit statusMessage("【配置】设置RN节点STOF接收参数...");
    
    // 设置STOF消息接收周期（与CC节点保持一致）
    if (MSG_STOF_Period(hRnNode, LARGE_PERIOD) != OK) {
        //emit errorMessage("【错误】设置RN节点STOF周期失败");
        return false;
    }
    
    // 设置STOF接收门限（容错范围）
    if (MSG_RECV_STOF_limitPer(hRnNode, PERIOD_LIMIT) != OK) {
        //emit errorMessage("【错误】设置RN节点STOF接收门限失败");
        return false;
    }
    
    //emit statusMessage(QString("【配置】RN节点STOF参数配置成功，周期：%1微秒，容错范围：%2微秒").arg(LARGE_PERIOD).arg(PERIOD_LIMIT));
    return true;
}

// 启动RN节点STOF消息接收
bool RNnodeWork::startRnNodeStofRecv() {
    //emit statusMessage("【启动】启动RN节点STOF消息接收...");
    
    // 启用消息接收功能，为接收STOF消息做准备
    if (MSG_RECV_Ctrl(hRnNode, ABILITY_ENABLE) != OK) {
        //emit errorMessage("【错误】启用RN节点消息接收失败");
        return false;
    }
    
    // 设置RN节点打开状态标志，表示STOF接收已成功启动
    bRnNodeOpened = true;
    //emit statusMessage("【启动】RN节点STOF消息接收已启动");
    return true;
}

// 停止RN节点STOF消息接收
bool RNnodeWork::stopRnNodeStofRecv() {
    //emit statusMessage("【停止】停止RN节点STOF消息接收...");
    
    // 禁用消息接收功能
    if (MSG_RECV_Ctrl(hRnNode, ABILITY_DISABLE) != OK) {
        //emit errorMessage("【错误】停止RN节点消息接收失败");
        return false;
    }
    
    //emit statusMessage("【停止】RN节点STOF消息接收已停止");
    return true;
}

// 关闭RN节点并释放资源
void RNnodeWork::closeRnNode() {
    //emit statusMessage("【清理】关闭RN节点并释放资源...");
    
    // 关闭RN节点
    if (hRnNode != nullptr) {
        // 禁用消息接收
        MSG_RECV_Ctrl(hRnNode, ABILITY_DISABLE);
        // 关闭节点设备
        Mil1394_Close(hRnNode);
        hRnNode = nullptr;
        bRnNodeOpened = false;
    }
    
    //emit statusMessage("【清理】RN节点已关闭，资源已释放");
}

// // 验证STOF消息的有效性和正确性
// bool RNnodeWork::validateStofMessage(const _MsgSTOF &stofMsg) {

// }

// // 更新STOF消息统计信息
// void RNnodeWork::updateStofStats(bool messageReceived) {

// }

// 获取RN节点的消息计数
void RNnodeWork::getRnMessageCounts() {
    if (hRnNode == nullptr) {
        return;
    }

    for (int i = BusResetCNT; i <= ERR_TopicIDCNT; i++) {
        Mil1394_MSG_Cnt_Get(hRnNode, i, &meassageCounts[i]);
    }
}

// 处理消息
void RNnodeWork::processMessage(_RcvMsgList *msgList) {
    // 确保是一条可用的消息
    if (msgList == nullptr || msgList->msgstateAddr == nullptr) {
        return;
    }
    
    // 获取消息状态
    _MsgState* msgState = msgList->msgstateAddr;
    
    // 根据消息类型进行处理
    switch (msgState->MessageTYPE) {
        case MSG_BUSRESET:
            std::cout << "【总线复位消息】接收到总线复位消息" << std::endl;
            break;
            
        case MSG_STOF: 
            std::cout << "【STOF消息】接收到STOF消息" << std::endl;
            // 如果需要获取STOF消息具体内容，可以调用MSG_RECV_Packet_STOF
            _MsgSTOF stofMsg;
            if (MSG_RECV_Packet_STOF(hRnNode, &stofMsg, msgState) == OK) {
                std::cout << "  Payload0: 0x" << std::hex << stofMsg.STOFPayload0 << std::dec << std::endl;
                std::cout << "  Payload3: 0x" << std::hex << stofMsg.STOFPayload3 << std::dec << std::endl;
            }
            break;
            
        case MSG_ASYNC: 
            std::cout << "【异步消息】接收到异步消息" << std::endl;
            // 如果需要获取异步消息具体内容，可以调用MSG_RECV_Packet_Asyn
            _MsgAsyn asyncMsg;
            if (MSG_RECV_Packet_Asyn(hRnNode, msgState->MessageID, &asyncMsg, msgState) == OK) {
                std::cout << "  MessageID: 0x" << std::hex << asyncMsg.MessageID << std::dec << std::endl;
                std::cout << "  PayloadLen: " << asyncMsg.payloadLen << " 字节" << std::endl;
            }
            break;
            
        case MSG_EVENT:
            std::cout << "【事件消息】接收到事件消息，ID: 0x" << std::hex << msgState->MessageID << std::dec << std::endl;
            break;
            
        default:
            std::cout << "【未知消息】接收到未知类型消息，类型: " << msgState->MessageTYPE << std::endl;
            break;
    }
    
}

// 轮询处理消息
void RNnodeWork::pollAndProcessMessages() {
    // 检查是否有消息待处理
    TNFU32 msgCount = 0;
    _RcvMsgList* msgList = nullptr;
    _RcvMsgList* msgLastList = nullptr;
    
    // 获取消息列表
    if (MSG_RECV_list(RN_NODE_NO, &msgCount, &msgList, &msgLastList) == OK) {
        if (msgCount > 0) {
            //std::cout << "【消息队列】检测到 " << msgCount << " 条消息待处理" << std::endl;
            
            // 遍历消息列表并处理每条消息
            _RcvMsgList* currentMsg = msgList;
            for (TNFU32 i = 0; i < msgCount && currentMsg != nullptr; i++) {
                processMessage(currentMsg);
                currentMsg = currentMsg->next;
            }
        } 
        else {
            // 没有消息待处理
            //std::cout << "【消息队列】当前无消息待处理" << std::endl;
        }
    } else {
        //std::cout << "【错误】获取消息列表失败" << std::endl;
    }
}

// 线程运行函数
void RNnodeWork::run() {
    // 初始化RN节点（接收方），确保接收方先就绪
    if (!initRnNode()) {
        //throw "RN节点初始化失败";
    }
        
    // 配置RN节点STOF接收参数，设置接收周期和容错范围
    if (!configRnNodeStof()) {
        //throw "RN节点STOF配置失败";
    }
        
    // 启动RN节点STOF接收功能，准备接收来自CC节点的消息
    if (!startRnNodeStofRecv()) {
        //throw "RN节点STOF接收启动失败";
    }
        
    // 初始化成功，开始接收
    //emit statusMessage("【成功】RN节点初始化完成，开始接收STOF消息...");
        
    // 进入消息接收主循环
    while (!bStopRequested) {
        // 尝试接收STOF消息，超时时间设为周期的2倍（考虑到大周期可能较大）
        // 超时参数转换为毫秒单位
        //receiveStofMessage(LARGE_PERIOD * 2 / 1000);
        // 处理接收消息
        pollAndProcessMessages();

        // 处理计数
        getRnMessageCounts();
    }
        
    // 清理资源
    closeRnNode();
}
