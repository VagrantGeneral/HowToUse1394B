#ifndef CCNODEWORK_H
#define CCNODEWORK_H

#include "Mil1394_Emu_XT.h"
#include "messagestats.h"
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

    // 配置CC节点Asyn消息

    
    // 启动CC节点STOF消息发送
    bool startCcNodeStofSend();
    
    // 停止CC节点STOF消息发送
    bool stopCcNodeStofSend();
    
    // 关闭CC节点并释放资源
    void closeCcNode();

protected:
    // 线程运行函数
    void run() override;

signals:

private:
    _MsgSTOF            cc2RnStofPackage;               // CC到RN的STOF消息包
    _TNF_STOFCFG_Struct cc2RnStofCfg;                   // CC到RN的STOF配置结构体
    _TNF_Node_Struct*   hCcNode = nullptr;              // CC节点句柄
    bool                bCcNodeOpened = false;          // CC节点打开状态标志
    bool                bStopRequested = false;         // 停止请求标志
};

#endif // CCNODEWORK_H
