#ifndef SIGNALINGSERVER_H
#define SIGNALINGSERVER_H
#include <QObject>
#include <SocketIO/sio_client.h> // Make sure to link socket.io-client-cpp correctly
#include <string>

class SignalingServer : public QObject
{
    Q_OBJECT
public:
    explicit SignalingServer(QObject *parent = nullptr);
    void connectToServer(const std::string& url);
    void sendSDP( const QString& sdp);
    void sendICECandidate(const std::string& targetId, const std::string& candidate);
    void asnwerSocket();
signals:
    void sdpReceived(const QString sdp);
    void iceCandidateReceived(const std::string& senderId, const std::string& candidate);

private:
    sio::client socketClient;
    void setupSocketHandlers();
};

#endif // SIGNALINGSERVER_H

