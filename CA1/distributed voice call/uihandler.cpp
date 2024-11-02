#include "uihandler.h"
#include "qdebug.h"

UIHandler::UIHandler(QObject *parent)
    : QObject{parent}
{
    webRTC = new WebRTC();
    signalingServerHandler = new SignalingServer();
    signalingServerHandler->connectToServer("http://localhost:3000");

}

void UIHandler::offer()
{
    webRTC->init("EOT-x" , true);
    connect(webRTC , &WebRTC::sdpGenerated, signalingServerHandler , &SignalingServer::sendSDP);
    connect(signalingServerHandler, &SignalingServer::sdpReceived , this,&UIHandler::setAnswerSdp);
    webRTC->addPeer("4");
    webRTC->generateOfferSDP("4");
}

void UIHandler::answer()
{
    webRTC->init("EOT-x" , false);
    bool a  = connect(signalingServerHandler, &SignalingServer::sdpReceived , this,&UIHandler::setAnswerSdp);
    connect(webRTC , &WebRTC::sdpGenerated, signalingServerHandler , &SignalingServer::sendSDP);
    qDebug()<<a;
    signalingServerHandler->asnwerSocket();
    webRTC->addPeer("4");
    webRTC->generateAnswerSDP("4");

}

void UIHandler::setAnswerSdp(const QString &sdp)
{
    qDebug()<<"asdfghjkl";
    webRTC->setRemoteDescription("4",sdp);

}
