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
        ui->ccSTOFsend->setText(QString::number(rnmessagecount[STOF_SendCNT]));
        ui->ccSTOFrecv->setText(QString::number(rnmessagecount[STOF_RecvCNT]));
        ui->ccSTOFvpc->setText(QString::number(rnmessagecount[ERR_STOF_VPCCNT]));
        ui->ccSTOFcrc->setText(QString::number(rnmessagecount[ERR_STOF_DCRCCNT]));
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

}

MainWidget::~MainWidget() {
    delete ui;
}

