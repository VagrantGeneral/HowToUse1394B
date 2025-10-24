/**
 * @file AsyncPeriodicDemo.h
 * @brief 1394B通信异步消息周期性收发演示程序头文件
 * 
 * 本程序实现了CC(Controller Controller)节点以1秒周期周期性发送异步消息，
 * 以及RN(Remote Node)节点接收并处理异步消息的功能。同时包含了消息接收统计
 * 和验证逻辑，可实时显示通信状态和性能指标。
 */

#ifndef ASYNC_PERIODIC_DEMO_H
#define ASYNC_PERIODIC_DEMO_H

#include <QThread>  // Qt线程类，用于实现多线程操作
#include <QDebug>   // Qt调试输出类

#ifdef _WIN32
    #include <windows.h>  // Windows系统API
#else
    #include <sys/time.h>  // Linux/Unix时间函数
#endif

#include "XT1394B/Mil1394_Emu_XT.h"  // 1394B卡驱动库头文件

/**
 * @struct AsyncStats
 * @brief 异步消息统计结构体
 * 
 * 用于全面统计和记录异步消息的接收情况，包括消息数量统计、通信质量分析
 * 和性能指标计算。该结构体在RN节点接收处理过程中被实时更新，用于监控
 * 1394B总线通信的可靠性和稳定性。
 */
typedef struct {
    int totalReceived;         // 总共接收到的异步消息数
    int expectedMessages;      // 期望接收到的异步消息数
    int errorCount;            // 错误消息计数
    int timeoutCount;          // 超时计数
    unsigned int lastCounter;  // 上一次接收到的计数器值
    double startTime;          // 通信开始时间（秒）
    double endTime;            // 通信结束时间（秒）
    double totalDuration;      // 总通信时长（秒）
    double packetLossRate;     // 丢包率
    double messageRate;        // 消息接收速率（消息/秒）
} AsyncStats;

/**
 * @brief 异步消息周期性通信演示类
 * @details 继承自QThread，实现CC节点周期性发送异步消息，RN节点接收异步消息的功能
 */
class AsyncPeriodicDemo : public QThread
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit AsyncPeriodicDemo(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~AsyncPeriodicDemo();

    /**
     * @brief 初始化CC节点
     * @return 是否成功初始化CC节点
     */
    bool initCcNode();
    
    /**
     * @brief 配置CC节点的异步消息
     * @return 是否成功配置异步消息
     */
    bool configCcNodeAsync();
    
    /**
     * @brief 启动CC节点异步消息发送
     * @return 是否成功启动异步发送
     */
    bool startCcNodeAsyncSend();
    
    /**
     * @brief 初始化RN节点
     * @return 是否成功初始化RN节点
     */
    bool initRnNode();
    
    /**
     * @brief 配置RN节点的异步消息接收
     * @return 是否成功配置异步消息接收
     */
    bool configRnNodeAsync();
    
    /**
     * @brief 启动RN节点异步消息接收
     * @return 是否成功启动异步接收
     */
    bool startRnNodeAsyncRecv();
    
    /**
     * @brief 关闭节点并释放资源
     */
    void closeNodes();

protected:
    /**
     * @brief 线程运行函数
     * @details 实现异步消息周期性通信的主流程
     */
    void run() override;

private:
    /**
     * @brief 获取当前系统时间（秒）
     * @return 当前系统时间（秒）
     */
    double getCurrentTime();
    
    /**
     * @brief 验证异步消息的有效性和正确性
     * @param asyncMsg 接收到的异步消息结构体引用
     * @return 消息是否通过验证
     */
    bool validateAsyncMessage(const _MsgAsyn& asyncMsg);
    
    /**
     * @brief 更新异步消息统计信息
     * @param messageReceived 指示是否成功接收到有效消息
     */
    void updateAsyncStats(bool messageReceived);
    
    /**
     * @brief 显示异步消息统计结果
     */
    void displayAsyncStats();
    
    /**
     * @brief 接收并处理异步消息
     * @param timeoutMs 超时时间（毫秒）
     * @return 是否成功接收到有效异步消息
     */
    bool receiveAsyncMessage(int timeoutMs);

private:
    // 节点定义
    static const int CC_NODE_NO = 0;   // CC节点编号
    static const int RN_NODE_NO = 1;   // RN节点编号
    static const int DEVICE_NO = 0;    // 设备编号

    // 通信参数定义
    static const int COMM_SPEED = PORTSPEED_S400;  // 通信速率：400Mbps
    static const int LARGE_PERIOD = 1000000;       // 大周期：1秒(1,000,000微秒)
    static const int PERIOD_LIMIT = 50000;         // 容错范围：50毫秒(50,000微秒)
    static const int ASYNC_MSG_ID = 0x1001;        // 异步消息ID
    static const int ASYNC_PAYLOAD_LEN = 256;      // 异步消息载荷长度

    // 节点句柄和状态
    _TNF_Node_Struct* hCcNode;        // CC节点句柄
    _TNF_Node_Struct* hRnNode;        // RN节点句柄
    bool bCcNodeOpened;               // CC节点打开状态标志
    bool bRnNodeOpened;               // RN节点打开状态标志

    // 异步消息相关结构体
    _MsgAsyn cc2RnAsyncPackage;       // CC到RN的异步消息包
    _TNF_ASYNCCFG_Struct cc2RnAsyncCfg; // CC到RN的异步消息配置结构体
    _MsgAsyn receivedAsyncPackage;    // 接收的异步消息包

    // 统计信息
    AsyncStats asyncStats;            // 异步消息统计信息实例
};

#endif // ASYNC_PERIODIC_DEMO_H