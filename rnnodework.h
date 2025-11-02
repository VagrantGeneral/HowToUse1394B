#ifndef RNNODEWORK_H
#define RNNODEWORK_H

#include "Mil1394_Emu_XT.h"
#include "messagestats.h"
#include <QtGlobal>
#include <QThread>

class RNnodeWork : public QThread {
    Q_OBJECT
public:
    explicit RNnodeWork(QObject *parent = nullptr);

public slots:
    // 初始化RN节点
    bool initRnNode();

    // 配置RN节点的STOF接收参数
    bool configRnNodeStof();

    // 配置RN节点的Asyn发送
    bool configRnNodeAsynSend();

    // 配置RN节点的Asyn接收
    bool configRnNodeAsynRecv();

    // 启动RN节点STOF/Asyn消息接收
    bool startRnNodeStofRecv();

    // 停止RN节点STOF/Asyn消息接收
    bool stopRnNodeStofRecv();

    // 启动RN节点Asyn消息发送
    bool startRnNodeAsynSend();

    // 停止RN节点Asyn消息发送
    bool stopRnNodeAsynSend();

    // 关闭RN节点并释放资源
    void closeRnNode();

    // 获取RN节点的消息计数
    void getRnMessageCounts();

    //
    void setAsyncContent(const TNFU32 data[], int length);

    // 处理消息
    void processMessage(_RcvMsgList* msgList);

    // 轮询处理消息
    void pollAndProcessMessages();

protected:
    // 线程运行函数
    void run() override;

signals:
    //
    void updateMessageCounts(TNFU32 rnmessagecount[]);

    //
    void updateStateText(QString statestr);
private:
    TNFU32 rnMeassageCounts[ERR_TopicIDCNT + 1] = {0};  // RN节点的所有消息计数
    _MsgSTOF            receivedStofPackage;            // RN节点接收的STOF消息包
    _MsgAsyn            receivedAsynPackage;            // RN节点接收的Asyn消息包
    _MsgAsyn            rn2CcAsynPackage;               // RN节点发送到RN节点的Asyn消息包
    _TNF_ASYNCCFG_Struct rn2CcAsynCfg;                  // RN节点发送到RN节点的Asyn配置结构体

    TNFU32              deviceNo = DEVICE_NO;           //
    TNFU32              rnNodeNo = RN_NODE_NO;          //
    _TNF_Node_Struct*   hRnNode = nullptr;              // RN节点句柄
    bool                bRnNodeOpened = false;          // RN节点打开状态标志
    bool                bStopRequested = false;         // 停止请求标志
};

#endif // RNNODEWORK_H
