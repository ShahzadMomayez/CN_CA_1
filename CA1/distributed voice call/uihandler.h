#ifndef UIHANDLER_H
#define UIHANDLER_H

#include "signalingserver.h"
#include "webrtc.h"
#include <QObject>

class UIHandler : public QObject
{
    Q_OBJECT
public:
    explicit UIHandler(QObject *parent = nullptr);
    Q_INVOKABLE void offer();
    Q_INVOKABLE void answer();

private:
    WebRTC *webRTC;
    SignalingServer* signalingServerHandler;
public Q_SLOTS:
    void setAnswerSdp(const QString& sdp);
signals:
};

#endif // UIHANDLER_H
