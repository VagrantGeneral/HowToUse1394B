#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget) {
    ui->setupUi(this);
    // code
    
    ccnodeworker = new CCnodeWork(this);
    rnnodeworker = new RNnodeWork(this);

    // SIGNAL--->SLOT
    //
    connect(ui->startbtn_, &QPushButton::clicked, this, [=]() {
        rnnodeworker->start();
        ccnodeworker->start();
    });

    //
    connect(ccnodeworker, &CCnodeWork::updateMessageCounts, this, [=](TNFU32 ccmessagecount[]) {
        // STOF
        ui->ccSTOFsend->setText(QString::number(ccmessagecount[STOF_SendCNT]));
        ui->ccSTOFrecv->setText(QString::number(ccmessagecount[STOF_RecvCNT]));
        ui->ccSTOFvpc->setText(QString::number(ccmessagecount[ERR_STOF_VPCCNT]));
        ui->ccSTOFcrc->setText(QString::number(ccmessagecount[ERR_STOF_DCRCCNT]));
        // Asyn
        ui->ccAsynsend->setText(QString::number(ccmessagecount[Asyn_SendCNT]));
        ui->ccAsynrecv->setText(QString::number(ccmessagecount[Asyn_RecvCNT]));
        ui->ccAsyndcrc->setText(QString::number(ccmessagecount[ERR_Asyn_DCRCCNT]));
        ui->ccAsynmsgid->setText(QString::number(ccmessagecount[ERR_Asyn_MsgIDCNT]));
        ui->ccAsynvpc->setText(QString::number(ccmessagecount[ERR_Asyn_VPCCNT]));
        ui->ccAsynappvpc->setText(QString::number(ccmessagecount[ERR_Asyn_SVPCCNT]));
    });

    //
    connect(rnnodeworker, &RNnodeWork::updateMessageCounts, this, [=](TNFU32 rnmessagecount[]) {
        // STOF
        ui->rnSTOFsend->setText(QString::number(rnmessagecount[STOF_SendCNT]));
        ui->rnSTOFrecv->setText(QString::number(rnmessagecount[STOF_RecvCNT]));
        ui->rnSTOFvpc->setText(QString::number(rnmessagecount[ERR_STOF_VPCCNT]));
        ui->rnSTOFcrc->setText(QString::number(rnmessagecount[ERR_STOF_DCRCCNT]));
        // Asyn
        ui->rnAsynsend->setText(QString::number(rnmessagecount[Asyn_SendCNT]));
        ui->rnAsynrecv->setText(QString::number(rnmessagecount[Asyn_RecvCNT]));
        ui->rnAsyndcrc->setText(QString::number(rnmessagecount[ERR_Asyn_DCRCCNT]));
        ui->rnAsynmsgid->setText(QString::number(rnmessagecount[ERR_Asyn_MsgIDCNT]));
        ui->rnAsynvpc->setText(QString::number(rnmessagecount[ERR_Asyn_VPCCNT]));
        ui->rnAsynappvpc->setText(QString::number(rnmessagecount[ERR_Asyn_SVPCCNT]));
    });

    //
    connect(ccnodeworker, &CCnodeWork::updateStateText, this, [=](QString statestr) {
        ui->ccTextBrow_->append(statestr);
    });

    //
    connect(rnnodeworker, &RNnodeWork::updateStateText, this, [=](QString statestr) {
        ui->rnTextBrow_->append(statestr);
    });

    //
    //
    connect(ui->ccstofdatasetbtn_, &QPushButton::clicked, this, [=]() {
        //
        TNFU32 load0str = stringToTNFU32((ui->loadedit0_->text()).toStdString());  // toStdString();
        TNFU32 load1str = stringToTNFU32((ui->loadedit1_->text()).toStdString());
        TNFU32 load2str = stringToTNFU32((ui->loadedit2_->text()).toStdString());
        TNFU32 load3str = stringToTNFU32((ui->loadedit3_->text()).toStdString());
        TNFU32 load4str = stringToTNFU32((ui->loadedit4_->text()).toStdString());
        TNFU32 load5str = stringToTNFU32((ui->loadedit5_->text()).toStdString());
        TNFU32 load6str = stringToTNFU32((ui->loadedit6_->text()).toStdString());
        TNFU32 load7str = stringToTNFU32((ui->loadedit7_->text()).toStdString());
        TNFU32 load8str = stringToTNFU32((ui->loadedit8_->text()).toStdString());

        ccnodeworker->setStofContent(load0str, load1str, load2str,
                                     load3str, load4str, load5str,
                                     load6str, load7str, load8str);
    });

    //
    connect(ui->ccasyndatasetbtn_, &QPushButton::clicked, this, [=]() {
        //
        std::string datastr = (ui->ccAsynTextedit_->toPlainText()).toStdString();
        TNFU32 asyndata[100];
        //
        int size = this->stringToTNFU32Array(datastr, asyndata, 100);

        //
        ccnodeworker->setAsyncContent(asyndata, 100);
    });

    //
    connect(ui->rnasyndatasetbtn_, &QPushButton::clicked, this, [=]() {
        //
        std::string datastr = (ui->rnAsynTextedit_->toPlainText()).toStdString();
        TNFU32 asyndata[100];
        //
        int size = this->stringToTNFU32Array(datastr, asyndata, 100);

        //
        rnnodeworker->setAsyncContent(asyndata, 100);
    });

}

MainWidget::~MainWidget() {
    delete ui;
}

//
TNFU32 MainWidget::stringToTNFU32(const std::string &str) {
    // 处理十六进制字符串（以0x或0X开头）
    if (str.length() >= 2 && (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X")) {
        return static_cast<TNFU32>(std::stoull(str.substr(2), nullptr, 16));
    }

    // 处理二进制字符串（以0b或0B开头）
    if (str.length() >= 2 && (str.substr(0, 2) == "0b" || str.substr(0, 2) == "0B")) {
        return static_cast<TNFU32>(std::stoull(str.substr(2), nullptr, 2));
    }

    // 处理十进制字符串
    return static_cast<TNFU32>(std::stoull(str, nullptr, 10));
}

size_t MainWidget::stringToTNFU32Array(const std::string &str, TNFU32 data[], size_t maxElements) {
    // 计算字符串长度（以字节为单位）
    size_t strLength = str.length();

    // 计算需要的TNFU32单元数量（每个TNFU32可以存储4个字节）
    size_t requiredUnits = (strLength + 3) / 4;  // 向上取整

    // 确保不超过目标数组的最大容量
    size_t unitsToCopy = (requiredUnits < maxElements) ? requiredUnits : maxElements;

    // 清零所有数据单元
    memset(data, 0, sizeof(TNFU32) * maxElements);

    // 将字符串内容复制到TNFU32数组中
    const char* strPtr = str.c_str();
    for (size_t i = 0; i < unitsToCopy; ++i) {
        // 每次处理4个字节
        TNFU32 dataUnit = 0;
        size_t bytesToCopy = (strLength - i * 4 >= 4) ? 4 : (strLength - i * 4);

        // 将字节复制到TNFU32单元中（小端序）
        for (size_t j = 0; j < bytesToCopy; ++j) {
            dataUnit |= (static_cast<TNFU32>(static_cast<unsigned char>(strPtr[i * 4 + j])) << (j * 8));
        }

        data[i] = dataUnit;
    }

    return unitsToCopy;
}

//int MainWidget::stringToAsyncMessage(_MsgAsyn &asyncMsg, const std::string &content) {
//    // 计算字符串长度（以字节为单位）
//    size_t contentLength = content.length();

//    // 计算需要的TNFU32单元数量（每个TNFU32可以存储4个字节）
//    size_t requiredUnits = (contentLength + 3) / 4;  // 向上取整

//    // 确保不超过消息数据数组的最大容量（502个TNFU32单元）
//    size_t unitsToCopy = (requiredUnits < 502) ? requiredUnits : 502;

//    // 设置负载长度（以字节为单位）
//    asyncMsg.payloadLen = static_cast<TNFU32>(contentLength);

//    // 清零所有数据单元
//    memset(asyncMsg.msgData, 0, sizeof(asyncMsg.msgData));

//    // 将字符串内容复制到消息数据中
//    const char* contentPtr = content.c_str();
//    for (size_t i = 0; i < unitsToCopy; ++i) {
//        // 每次处理4个字节
//        TNFU32 dataUnit = 0;
//        size_t bytesToCopy = (contentLength - i * 4 >= 4) ? 4 : (contentLength - i * 4);

//        // 将字节复制到TNFU32单元中（小端序）
//        for (size_t j = 0; j < bytesToCopy; ++j) {
//            dataUnit |= (static_cast<TNFU32>(static_cast<unsigned char>(contentPtr[i * 4 + j])) << (j * 8));
//        }

//        asyncMsg.msgData[i] = dataUnit;
//    }

//    return requiredUnits;
//}













