#ifndef SIGNALINGSERVER_H
#define SIGNALINGSERVER_H

#include <QObject>
#include <SocketIO/sio_client.h>
#include <string>

class SignalingServer : public QObject
{
    Q_OBJECT
public:
    explicit SignalingServer(QObject *parent = nullptr);
    void connectToServer(const std::string& url);
    //void sendSDP(const QString& sdp);
    void sendSDPOffer(const QString& sdp);
    void sendSDPAnswer(const QString& sdp);
    void sendICECandidate(const std::string& targetId, const std::string& candidate);
    void answerSocket();

signals:
    void sdpReceived(const QString sdp);
    void sdpOfferReceived(const QString sdp);
    void sdpAnswerReceived(const QString sdp);
 // void iceCandidateReceived(const std::string& senderId, const std::string& candidate);
    void iceCandidateReceived(const QString& senderId, const QString& candidate);

private:
    sio::client socketClient;
    void setupSocketHandlers();
};

#endif // SIGNALINGSERVER_H
