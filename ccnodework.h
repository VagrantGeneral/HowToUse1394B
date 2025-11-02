#ifndef CCNODEWORK_H
#define CCNODEWORK_H

#include "Mil1394_Emu_XT.h"
#include "messagestats.h"
#include <QtGlobal>
#include <QThread>

class CCnodeWork : public QThread {
    Q_OBJECT
public:
    explicit CCnodeWork(QObject *parent = nullptr);

public slots:
    // 初始化CC节点
    bool initCcNode();
    
    // 配置CC节点STOF消息
    bool configCcNodeStof();

    // 配置CC节点Asyn消息发送
    bool configCcNodeAsynSend();

    // 配置CC节点Asyn消息接收
    bool configCcNodeAsynRecv();
    
    // 启动CC节点STOF消息发送
    bool startCcNodeStofSend();
    
    // 停止CC节点STOF消息发送
    bool stopCcNodeStofSend();

    // 启动CC节点Asyn消息发送
    bool startCcNodeAsynSend();

    // 启动CC节点Asyn消息接收
    bool startCcNodeAsynRecv();

    // 停止CC节点Asyn消息发送
    bool stopCcNodeAsynSend();

    // 停止CC节点Asyn消息接收
    bool stopCcNodeAsynRecv();
    
    // 关闭CC节点并释放资源
    void closeCcNode();

    // 设置STOF消息内容
    void setStofContent(TNFU32 payload0, TNFU32 payload1, TNFU32 payload2, TNFU32 payload3,
                        TNFU32 payload4, TNFU32 payload5, TNFU32 payload6, TNFU32 payload7, TNFU32 payload8);

    // 设置Asyn消息内容
    void setAsyncContent(const TNFU32 data[], int length);

    // 获取Cc节点的消息计数
    void getCcMessageCounts();

    // 处理CC节点接收到的消息
    void processMessage(_RcvMsgList* msgList);

    // 轮询处理消息
    void pollAndProcessMessages();

protected:
    // 线程运行函数
    void run() override;

signals:
    //
    void updateMessageCounts(TNFU32 ccmessagecount[]);

    //
    void updateStateText(QString statestr);
private:
    TNFU32 ccMeassageCounts[ERR_TopicIDCNT + 1] = {0};  // CC节点的消息计数
    _MsgSTOF            cc2RnStofPackage;               // CC节点发送到RN节点的STOF消息包
    _TNF_STOFCFG_Struct cc2RnStofCfg;                   // CC节点发送到RN节点的STOF配置结构体
    _MsgAsyn            cc2RnAsynPackage;               // CC节点发送到RN节点的Asyn消息包
    _TNF_ASYNCCFG_Struct cc2RnAsynCfg;                  // CC节点发送到RN节点的Asyn配置结构体
    _MsgAsyn            receivedAsynPackage;            // CC节点接收到的Asyn消息包

    TNFU32              deviceNo = DEVICE_NO;           //
    TNFU32              ccNodeNo = CC_NODE_NO;          //
    _TNF_Node_Struct*   hCcNode = nullptr;              // CC节点句柄
    bool                bCcNodeOpened = false;          // CC节点打开状态标志
    bool                bStopRequested = false;         // 停止请求标志
};

#endif // CCNODEWORK_H
