#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "ccnodework.h"
#include "rnnodework.h"
#include <QWidget>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget {
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    //For STOF PayLoad
    TNFU32 stringToTNFU32(const std::string& str);

    //For Asyn Msg
    //int stringToAsyncMessage(_MsgAsyn& asyncMsg, const std::string& content);
    size_t stringToTNFU32Array(const std::string& str, TNFU32 data[], size_t maxElements);

private:
    Ui::MainWidget *ui;
    CCnodeWork* ccnodeworker = nullptr;
    RNnodeWork* rnnodeworker = nullptr;
};
#endif // MAINWIDGET_H
