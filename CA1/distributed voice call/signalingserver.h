#ifndef SIGNALINGSERVER_H
#define SIGNALINGSERVER_H

#include <QObject>
#include <SocketIO/sio_client.h> // Make sure to link socket.io-client-cpp correctly

class signalingserver : public QObject
{
    Q_OBJECT
public:
    explicit signalingserver(QObject *parent = nullptr);

    // Connect to the signaling server
    void connectToServer(const std::string &url);

    // Send SDP and ICE candidate data to the server
    void emitSDP(const std::string &sdp, const std::string &targetId);
    void emitIceCandidate(const std::string &candidate, const std::string &targetId);

signals:
    // Signals to emit when we receive data from the server
    void sdpReceived(const QString &sdp, const QString &senderId);
    void iceCandidateReceived(const QString &candidate, const QString &senderId);

private:
    sio::client h; // Socket.IO client instance
    void onSDP(sio::event &event);
    void onIceCandidate(sio::event &event);
};

#endif // SIGNALINGSERVER_H
