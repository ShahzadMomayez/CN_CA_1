#include "signalingserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

signalingserver::signalingserver(QObject *parent) : QObject(parent) {
    // Bind events using direct lambda expressions
    h.socket()->on("sdp", [&](sio::event &event) { onSDP(event); });
    h.socket()->on("ice-candidate", [&](sio::event &event) { onIceCandidate(event); });
}

void signalingserver::connectToServer(const std::string &url) {
    h.connect(url);
    std::cout << "Connected to signaling server at " << url << std::endl;
}

void signalingserver::emitSDP(const std::string &sdp, const std::string &targetId) {
    QJsonObject message;
    message["type"] = "sdp"; // type should be fixed , we have to fix offerer and answerer so it would define each of them
                             // or you should fix it from two signals sending , or from the js file fix it . then wiring them is important
    message["targetId"] = QString::fromStdString(targetId);
    message["sdp"] = QString::fromStdString(sdp);

    std::string msg = QJsonDocument(message).toJson(QJsonDocument::Compact).toStdString();
    h.socket()->khosro_emit("sdp", msg);
}

void signalingserver::emitIceCandidate(const std::string &candidate, const std::string &targetId) {
    QJsonObject message;
    message["type"] = "ice-candidate";
    message["targetId"] = QString::fromStdString(targetId);
    message["candidate"] = QString::fromStdString(candidate);

    std::string msg = QJsonDocument(message).toJson(QJsonDocument::Compact).toStdString();
    h.socket()->khosro_emit("ice-candidate", msg);
}

void signalingserver::onSDP(sio::event &event) {
    auto msg = event.get_message()->get_map();
    QString sdp = QString::fromStdString(msg["sdp"]->get_string());
    QString senderId = QString::fromStdString(msg["senderId"]->get_string());

    emit sdpReceived(sdp, senderId);
}

void signalingserver::onIceCandidate(sio::event &event) {
    auto msg = event.get_message()->get_map();
    QString candidate = QString::fromStdString(msg["candidate"]->get_string());
    QString senderId = QString::fromStdString(msg["senderId"]->get_string());

    emit iceCandidateReceived(candidate, senderId);
}
