#include "signalingserver.h"
#include <iostream>
#include <QDebug>
#include <SocketIO/sio_client.h>
#include <string>

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
    // Handle incoming SDP offers
    socketClient.socket()->on("sdp_offer", [&](sio::event& ev) {
        std::string message = ev.get_message()->get_string();
        emit sdpOfferReceived(QString::fromStdString(message));
        qDebug() << "SDP Offer received:" << QString::fromStdString(message);

    });

    // Handle incoming SDP answers
    socketClient.socket()->on("sdp_answer", [&](sio::event& ev) {
        std::string message =  ev.get_message()->get_string();
        emit sdpAnswerReceived(QString::fromStdString(message));
        qDebug() << "SDP Answer received:" << QString::fromStdString(message);
    });

    socketClient.socket()->on("ice_candidate", [&](sio::event& ev) {
        std::string senderId = ev.get_message()->get_map()["senderId"]->get_string();
        std::string candidate = ev.get_message()->get_map()["candidate"]->get_string();
        emit iceCandidateReceived(QString::fromStdString(senderId), QString::fromStdString(candidate));
        qDebug() << "ICE Candidate received from:" << QString::fromStdString(senderId) << "Candidate:" << QString::fromStdString(candidate);
    });
}

void SignalingServer::sendSDPOffer(const QString& sdp) {
    sio::message::ptr msg = sio::object_message::create();
    msg->get_map()["sdp"] = sio::string_message::create(sdp.toStdString());

    socketClient.socket()->khosro_emit("sdp_offer", sdp.toStdString());
    qDebug() << "SDP Offer sent:" << sdp;
}

void SignalingServer::sendSDPAnswer(const QString& sdp) {
    sio::message::ptr msg = sio::object_message::create();
    msg->get_map()["sdp"] = sio::string_message::create(sdp.toStdString());
    socketClient.socket()->khosro_emit("sdp_answer", sdp.toStdString());
    qDebug() << "SDP Answer sent:" << sdp;
}


void SignalingServer::sendICECandidate(const std::string& targetId, const std::string& candidate)
{
    sio::message::ptr msg = sio::object_message::create();
    msg->get_map()["targetId"] = sio::string_message::create(targetId);
    msg->get_map()["candidate"] = sio::string_message::create(candidate);
    socketClient.socket()->khosro_emit("ice_candidate", msg);
    qDebug() << "ICE Candidate sent to:" << QString::fromStdString(targetId) << "Candidate:" << QString::fromStdString(candidate);
}

void SignalingServer::answerSocket()
{
    socketClient.socket()->khosro_emit("answer_event");
    qDebug() << "Answer socket emitted";
}
