#ifndef RNNODEWORK_H
#define RNNODEWORK_H

#include "Mil1394_Emu_XT.h"
#include "Mil1394_Emu_Addr.h"
#include "messagestats.h"
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

    // 启动RN节点STOF消息接收
    bool startRnNodeStofRecv();

    // 停止RN节点STOF消息接收
    bool stopRnNodeStofRecv();

    // 关闭RN节点并释放资源
    void closeRnNode();

    // // 验证STOF消息的有效性和正确性
    // bool validateStofMessage(const _MsgSTOF& stofMsg);

    // // 更新STOF消息统计信息
    // void updateStofStats(bool messageReceived);

    // 获取RN节点的消息计数
    void getRnMessageCounts();

    // 处理消息
    void processMessage(_RcvMsgList* msgList);

    // 轮询处理消息
    void pollAndProcessMessages();

protected:
    // 线程运行函数
    void run() override;

signals:

private:
    TNFU32 rnMeassageCounts[ERR_TopicIDCNT + 1] = {0};  // RN节点的消息计数
    // StofMessageStats    stofstats;                      // STOF消息统计信息实例
    _MsgSTOF            receivedStofPackage;            // 接收的STOF消息包
    _TNF_Node_Struct*   hRnNode = nullptr;              // RN节点句柄
    bool                bRnNodeOpened = false;          // RN节点打开状态标志
    bool                bStopRequested = false;         // 停止请求标志
};

#endif // RNNODEWORK_H
