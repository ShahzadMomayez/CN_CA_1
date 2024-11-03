#include "uihandler.h"
#include "audioinput.h"
#include "audiooutput.h"
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
    AudioInput * audioIput = new AudioInput();
    webRTC->init("EOT-x", true);
    connect(signalingServerHandler, &SignalingServer::sdpAnswerReceived, this, &UIHandler::setOfferSdp);

    connect(webRTC, &WebRTC::sdpGenerated, signalingServerHandler, &SignalingServer::sendSDPOffer);
    connect(webRTC, &WebRTC::trackIsOPen,audioIput, &AudioInput::start );
    connect(audioIput, &AudioInput::newDataAvailable, this, &UIHandler::sendData);
    webRTC->addPeer("answerer");
    webRTC->generateOfferSDP("answerer");
}

void UIHandler::answer()
{
    AudioOutput * audioOutput = new AudioOutput();
    webRTC->init("EOT-x", false);
    connect(webRTC, &WebRTC::sdpGenerated, signalingServerHandler, &SignalingServer::sendSDPAnswer);
    connect(signalingServerHandler, &SignalingServer::sdpOfferReceived, this, &UIHandler::setAnswerSdp);
    connect(webRTC, &WebRTC::trackIsOPen,audioOutput , &AudioOutput::start);
    connect(webRTC, &WebRTC::packetRecieved, audioOutput,&AudioOutput::addData);
    signalingServerHandler->answerSocket();

    webRTC->addPeer("offerer");
}

void UIHandler::setAnswerSdp(const QString &sdp)
{
    webRTC->setRemoteDescription("offerer", sdp);
}

void UIHandler::setOfferSdp(const QString &sdp)
{
    webRTC->setRemoteDescription("answerer",sdp);
}

void UIHandler::sendData(const QByteArray &data)
{
    webRTC->sendTrack("answerer",data);
}
