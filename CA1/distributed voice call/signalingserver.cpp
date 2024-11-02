#include "signalingserver.h"
#include <iostream>
#include <QDebug>
#include <iostream>


SignalingServer::SignalingServer(QObject *parent)
    : QObject(parent)
{
    setupSocketHandlers();
}

void SignalingServer::connectToServer(const std::string& url)
{
    socketClient.connect(url);
}

void SignalingServer::setupSocketHandlers()
{

    socketClient.socket()->on("sdp", [&](sio::event& ev) {
        std::string massage = ev.get_message()->get_string();
        emit sdpReceived(QString::fromStdString(massage));
        qDebug()<<"Asdasd";
    });

    socketClient.socket()->on("ice_candidate", [&](sio::event& ev) {
        std::string senderId = ev.get_message()->get_map()["senderId"]->get_string();
        std::string candidate = ev.get_message()->get_map()["candidate"]->get_string();
        emit iceCandidateReceived(senderId, candidate);
    });
}

void SignalingServer::sendSDP(const QString& sdp)
{
    socketClient.socket()->khosro_emit("sdp", sdp.toStdString());
}

void SignalingServer::sendICECandidate(const std::string& targetId, const std::string& candidate)
{
    sio::message::ptr msg = sio::object_message::create();
    msg->get_map()["targetId"] = sio::string_message::create(targetId);
    msg->get_map()["candidate"] = sio::string_message::create(candidate);
    socketClient.socket()->khosro_emit("ice_candidate", msg);
}

void SignalingServer::asnwerSocket()
{
    socketClient.socket()->khosro_emit("mewo");
}

