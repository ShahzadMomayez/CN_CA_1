#include "webrtc.h"
#include <QtEndian>
#include <QJsonDocument>
#define D qDebug()
static_assert(true);

#pragma pack(push, 1)
struct RtpHeader {
    uint8_t first;
    uint8_t marker:1;
    uint8_t payloadType:7;
    uint16_t sequenceNumber;
    uint32_t timestamp;
    uint32_t ssrc;
};
#pragma pack(pop)


WebRTC::WebRTC(QObject *parent)
    : QObject{parent},
    m_audio("Audio")
{
    connect(this, &WebRTC::gatheringComplited, [this] (const QString &peerID) {

        m_localDescription = descriptionToJson(m_peerConnections[peerID]->localDescription().value());
        Q_EMIT localDescriptionGenerated(peerID, m_localDescription);

        if (m_isOfferer)
            Q_EMIT this->offerIsReady(peerID, m_localDescription);
        else
            Q_EMIT this->answerIsReady(peerID, m_localDescription);
    });
}

WebRTC::~WebRTC() {
    m_peerConnections.clear();
    qDebug() << "---";
}



/**
 * ====================================================
 * ================= public methods ===================
 * ====================================================
 */


void WebRTC::init(const QString &id, bool isOfferer) {
    m_localId = id;
    m_isOfferer = isOfferer;

    // Create an instance of rtc::Configuration to Set up ICE configuration
    m_config.iceServers.emplace_back("stun:stun.l.google.com:19302"); // Add a STUN server
   // m_config.iceServers.emplace_back("turn:your.turn.server.here:3478?transport=udp"); // Add a TURN server

    // Initialize the audio stream configuration
    m_audio = rtc::Description::Audio();
    m_audio.addOpusCodec(m_payloadType);
    m_audio.addSSRC(m_ssrc, "voice");
    if (m_isOfferer) {
        m_audio.setDirection(rtc::Description::Direction::SendOnly);
    } else {
        m_audio.setDirection(rtc::Description::Direction::RecvOnly);
    }

    // Initialize Peer Connections
    m_peerConnections.clear();
    m_peerSdps.clear();
    m_peerTracks.clear();
}


void WebRTC::addPeer(const QString &peerId)
{
    // Create and add a new peer connection
    auto newPeer = std::make_shared<rtc::PeerConnection>(m_config);
    m_peerConnections.insert(peerId, newPeer);

    // Set up a callback for when the local description is generated
    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        QString sdp = QString::fromStdString(description.generateSdp());

        emit localDescriptionGenerated(peerId, sdp);

        // The local description should be emitted using the appropriate signals based on the peer's role (offerer or answerer)
        if (m_isOfferer) {
            emit offerIsReady(peerId, sdp);
        } else {
            emit answerIsReady(peerId, sdp);
        }
    });

    // Set up a callback for handling local ICE candidates
    newPeer->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        QString candidateStr = QString::fromStdString(candidate.candidate());
        QString sdpMid = QString::fromStdString(candidate.mid());
        emit localCandidateGenerated(peerId, candidateStr, sdpMid);
    });

    // Set up a callback for when the state of the peer connection changes
    newPeer->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        switch (state) {
        case rtc::PeerConnection::State::New:
            // Handle state New
            qDebug() << "Peer" << peerId << "is in state New.";
            break;
        case rtc::PeerConnection::State::Connecting:
            // Handle state Connecting
            break;
        case rtc::PeerConnection::State::Connected:
            // Handle state Connected
            emit openedDataChannel(peerId);
            break;
        case rtc::PeerConnection::State::Disconnected:
            // Handle state Disconnected
            emit closedDataChannel(peerId);
            break;
        case rtc::PeerConnection::State::Failed:
            // Handle state Failed
            emit closedDataChannel(peerId);
            break;
        case rtc::PeerConnection::State::Closed:
            // Handle state Closed
            emit closedDataChannel(peerId);
            break;
        }
    });

    // Set up a callback for monitoring the gathering state
    newPeer->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            emit gatheringComplited(peerId);
        }
    });

    // Set up a callback for handling incoming tracks
    newPeer->onTrack([this, peerId](std::shared_ptr<rtc::Track> track) {
        track->onFrame([this, peerId](rtc::binary data, rtc::FrameInfo frame) {
            const char* dataChar = reinterpret_cast<const char*>(data.data());
            emit incommingPacket(peerId, QByteArray(dataChar, static_cast<int>(data.size())), data.size());
        });
    });

    // Add an audio track to the peer connection
    if (m_isOfferer) {

        auto track =newPeer->addTrack(m_audio);
    }
}


// Set the local description for the peer's connection
void WebRTC::generateOfferSDP(const QString &peerId)
{
    auto peerConnection = m_peerConnections.value(peerId);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerId;
        return;
    }
    peerConnection->setLocalDescription(rtc::Description::Type::Offer,
                                        [this, peerId](const rtc::Description &desc) {
                                            m_localDescription = QString::fromStdString(descriptionToJson(desc));
                                            emit offerIsReady(peerId, m_localDescription);
                                        },
                                        [](const std::string &error) {
                                            qWarning() << "Failed to set local description:" << QString::fromStdString(error);
                                        }
                                        );



}

// Generate an answer SDP for the peer
void WebRTC::generateAnswerSDP(const QString &peerId)
{

}

// Add an audio track to the peer connection
void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName)
{
    // Add an audio track to the peer connection

    // Handle track events

   // track->onMessage([this, peerId] (rtc::message_variant data) {});

 //   track->onFrame([this] (rtc::binary frame, rtc::FrameInfo info) {

   // });

}

// Sends audio track data to the peer

void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{

    // Create the RTP header and initialize an RtpHeader struct


    // Create the RTP packet by appending the RTP header and the payload buffer


    // Send the packet, catch and handle any errors that occur during sending

}


/**
 * ====================================================
 * ================= public slots =====================
 * ====================================================
 */

// Set the remote SDP description for the peer that contains metadata about the media being transmitted
void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{

}

// Add remote ICE candidates to the peer connection
void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{

}


/*
 * ====================================================
 * ================= private methods ==================
 * ====================================================
 */

// Utility function to read the rtc::message_variant into a QByteArray
QByteArray WebRTC::readVariant(const rtc::message_variant &data)
{

}

// Utility function to convert rtc::Description to JSON format
QString WebRTC::descriptionToJson(const rtc::Description &description)
{

}

// Retrieves the current bit rate
int WebRTC::bitRate() const
{

}

// Set a new bit rate and emit the bitRateChanged signal
void WebRTC::setBitRate(int newBitRate)
{

}

// Reset the bit rate to its default value
void WebRTC::resetBitRate()
{

}

// Sets a new payload type and emit the payloadTypeChanged signal
void WebRTC::setPayloadType(int newPayloadType)
{

}

// Resets the payload type to its default value
void WebRTC::resetPayloadType()
{

}

// Retrieve the current SSRC value
rtc::SSRC WebRTC::ssrc() const
{

}

// Set a new SSRC and emit the ssrcChanged signal
void WebRTC::setSsrc(rtc::SSRC newSsrc)
{


}

// Reset the SSRC to its default value
void WebRTC::resetSsrc()
{

}

// Retrieve the current payload type
int WebRTC::payloadType() const
{
    return m_payloadType;
}


/**
 * ====================================================
 * ================= getters setters ==================
 * ====================================================
 */

bool WebRTC::isOfferer() const
{
    return m_isOfferer;
}

void WebRTC::setIsOfferer(bool newIsOfferer)
{
    m_isOfferer = newIsOfferer;
}

void WebRTC::resetIsOfferer()
{
    m_isOfferer = false;

}


