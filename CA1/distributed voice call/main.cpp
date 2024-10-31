#include <QCoreApplication>
#include "signalingserver.h"
#include <QMediaDevices>
#include <QDebug>
#include "audioinput.h"
#include "audiooutput.h"
#include "webrtc.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    SignalingServer signalingServer;

    const std::string serverUrl = "http://localhost:3000";
    const std::string callerId = "caller_01";
    const std::string receiverId = "receiver_01";

    signalingServer.connectToServer(serverUrl);

    QObject::connect(&signalingServer, &SignalingServer::sdpReceived,
                     [&](const std::string& senderId, const std::string& sdp) {
                         std::cout << "SDP received from: " << senderId << "\nSDP: " << sdp << std::endl;
                         // handleling received SDP here, potentially create an answer.
                     });

    QObject::connect(&signalingServer, &SignalingServer::iceCandidateReceived,
                     [&](const std::string& senderId, const std::string& candidate) {
                         std::cout << "ICE Candidate received from: " << senderId << "\nCandidate: " << candidate << std::endl;

                     });

    // Get the default audio input device
    QAudioDevice audioDevice = QMediaDevices::defaultAudioInput();
    AudioInput audioInput(audioDevice);

    // Example: sending an SDP offer
    std::string exampleSdp = "v=0...";
    signalingServer.sendSDP(receiverId, exampleSdp);

    return app.exec();
}
