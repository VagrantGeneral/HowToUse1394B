#include "ccnodework.h"

CCnodeWork::CCnodeWork(QObject *parent)
    : QThread(parent) {
    // code
    // 初始化STOF消息相关结构体
    memset(&cc2RnStofPackage, 0, sizeof(cc2RnStofPackage));
    memset(&cc2RnStofCfg, 0, sizeof(cc2RnStofCfg));
}

// 初始化CC节点
bool CCnodeWork::initCcNode() {
    //emit statusMessage("【初始化】打开CC节点...");
    
    // 打开CC节点设备，使用指定的设备号和节点号
    hCcNode = static_cast<_TNF_Node_Struct*>(Mil1394_XT_OPEN(nullptr, const_cast<TNFU32*>(&DEVICE_NO), CC_NODE_NO));
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

// 线程运行函数
void CCnodeWork::run() {
    // 初始化CC节点（发送方），准备发送STOF消息
    if (!initCcNode()) {
        //throw "CC节点初始化失败";
    }
        
    // 配置CC节点STOF发送参数，设置1秒大周期和消息内容
    if (!configCcNodeStof()) {
        //throw "CC节点STOF配置失败";
    }
        
    // 启动CC节点STOF发送功能，开始周期性发送消息
    if (!startCcNodeStofSend()) {
        //throw "CC节点STOF发送启动失败";
    }
        
    // 初始化成功，开始发送
    //emit statusMessage("【成功】CC节点初始化完成，开始周期性发送STOF消息...");
        
    // 进入消息发送主循环
    while (!bStopRequested) {
        // 线程休眠，让出CPU时间片
        QThread::msleep(100);
    }

    // 清理资源
    closeCcNode();
}
