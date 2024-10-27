#include "webrtc.h"
#include <QtEndian>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

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
        D <<"kargahi";
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
            D<<"gathering completed";
            emit gatheringComplited(peerId);
        }
    });


    // Set up a callback for handling incoming tracks
    newPeer->onTrack([this, peerId](std::shared_ptr<rtc::Track> track) {
        D<<"on track";
        // track->onFrame([this, peerId](rtc::binary data, rtc::FrameInfo frame) {
        //     const char* dataChar = reinterpret_cast<const char*>(data.data());
        //     emit incommingPacket(peerId, QByteArray(dataChar, static_cast<int>(data.size())), data.size());
        // });
    });

    // Add an audio track to the peer connection
    this->addAudioTrack(peerId , "Shazad");

}


// Set the local description for the peer's connection
void WebRTC::generateOfferSDP(const QString &peerId)
{
    auto peerConnection = m_peerConnections.value(peerId);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerId;
        return;
    }
    peerConnection->setLocalDescription(rtc::Description::Type::Offer);





}

// Generate an answer SDP for the peer
void WebRTC::generateAnswerSDP(const QString &peerId) {
    auto peerConnection = m_peerConnections.value(peerId);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerId;
        return;
    }
    peerConnection->setLocalDescription(rtc::Description::Type::Offer);
}


// Add an audio track to the peer connection
void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName) {
    auto peerConnection = m_peerConnections.value(peerId);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerId;
        return;
    }

    // Create an audio track description with the necessary parameters




    m_audio.addOpusCodec(m_payloadType); // Add Opus codec with specified payload type

    // Add SSRC
    m_audio.addSSRC(m_ssrc, "audio");

    // Add the audio track to the peer connection
    auto audioTrack = peerConnection->addTrack(m_audio);
    m_peerTracks.insert(peerId, audioTrack);

    // Handle track events
    audioTrack->onFrame([this, peerId](rtc::binary frame, rtc::FrameInfo info) {
        qDebug() << "Received audio frame from peer:" << peerId;
    });

    qDebug() << "Audio track added for peer:" << peerId;
}


// Sends audio track data to the peer
void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{

        auto peerConnection = m_peerConnections.value(peerId);
        if (!peerConnection) {
            qWarning() << "Peer connection not found for" << peerId;
            return;
        }

        auto audioTrack = m_peerTracks.value(peerId);
        if (!audioTrack) {
            qWarning() << "Audio track not found for" << peerId;
            return;
        }
        // SET HEADER
        RtpHeader header;
        header.ssrc = qToBigEndian(m_ssrc);
        header.first = 0x80;
        header.marker = 0;
        header.payloadType = m_payloadType;
        header.sequenceNumber = qToBigEndian(++m_sequenceNumber);
        header.timestamp = qToBigEndian(getCurrentTimestamp());
        // Convert QByteArray to rtc::binary


        // Send the audio data via the track


        qDebug() << "Audio data sent to peer:" <<  peerId;
}


/**
 * ====================================================
 * ================= public slots =====================
 * ====================================================
 */

// Set the remote SDP description for the peer that contains metadata about the media being transmitted
void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{

    auto peerConnection = m_peerConnections.value(peerID);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerID;
        return;
    }
    QString rawSdp = sdp;
    rtc::Description remoteDescription(rawSdp.toStdString(), (m_isOfferer)?rtc::Description::Type::Answer: rtc::Description::Type::Offer,rtc::Description::Role::ActPass);
    peerConnection->setRemoteDescription(remoteDescription);


}

// Add remote ICE candidates to the peer connection
void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{
    // you dont need this function
    auto peerConnection = m_peerConnections.value(peerID);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerID;
        return;
    }

    // Convert the candidate and sdpMid QStrings to std::string
    std::string candidateStr = candidate.toStdString();
    std::string sdpMidStr = sdpMid.toStdString();

    // Create the remote ICE candidate
    rtc::Candidate iceCandidate(candidateStr, sdpMidStr);

    try {
        // Add the remote candidate to the peer connection
        peerConnection->addRemoteCandidate(iceCandidate);
        qDebug() << "Remote candidate added for peer:" << peerID;
    } catch (const std::exception &e) {
        qWarning() << "Failed to add remote candidate: " << QString::fromStdString(e.what());
    }

}


/*
 * ====================================================
 * ================= private methods ==================
 * ====================================================
 */

// Utility function to read the rtc::message_variant into a QByteArray
QByteArray WebRTC::readVariant(const rtc::message_variant &data)
{
    QByteArray byteArray;

    if (std::holds_alternative<rtc::binary>(data)) {
        // If the data is of type rtc::binary
        const rtc::binary &binaryData = std::get<rtc::binary>(data);
        byteArray = QByteArray(reinterpret_cast<const char*>(binaryData.data()), static_cast<int>(binaryData.size()));
    } else if (std::holds_alternative<std::string>(data)) {
        // If the data is of type std::string
        const std::string &stringData = std::get<std::string>(data);
        byteArray = QByteArray::fromStdString(stringData);
    } else {
        qWarning() << "Unknown variant type.";
    }

    return byteArray;
}

// Utility function to convert rtc::Description to JSON format
QString WebRTC::descriptionToJson(const rtc::Description &description)
{
    // Use QJsonObject and QJsonDocument to create a JSON representation of the rtc::Description
    QJsonObject json;

    // Set the type of the SDP (Offer/Answer)
    json["type"] = QString::fromStdString(description.typeString()); // Using typeString method for type

    // Set the SDP string
    json["sdp"] = QString::fromStdString(description.generateSdp()); // Assuming cast to std::string for SDP

    // Convert QJsonObject to QString
    QJsonDocument doc(json);
    return QString(doc.toJson(QJsonDocument::Compact));
}

// Retrieves the current bit rate
int WebRTC::bitRate() const
{
    return m_bitRate;
}

// Set a new bit rate and emit the bitRateChanged signal
void WebRTC::setBitRate(int newBitRate)
{
    if (m_bitRate != newBitRate) {
        m_bitRate = newBitRate;
        emit bitRateChanged();
    }
}

// Reset the bit rate to its default value
void WebRTC::resetBitRate()
{
    m_bitRate = 48000; // Assuming 48000 is the default bit rate
    emit bitRateChanged();
}

// Sets a new payload type and emit the payloadTypeChanged signal
void WebRTC::setPayloadType(int newPayloadType)
{
    if (m_payloadType != newPayloadType) {
        m_payloadType = newPayloadType;
        emit payloadTypeChanged();
    }
}

// Resets the payload type to its default value
void WebRTC::resetPayloadType()
{
    m_payloadType = 111; // Assuming 111 is the default payload type
    emit payloadTypeChanged();
}

// Retrieve the current SSRC value
rtc::SSRC WebRTC::ssrc() const
{
    return m_ssrc;
}

// Set a new SSRC and emit the ssrcChanged signal
void WebRTC::setSsrc(rtc::SSRC newSsrc)
{
    if (m_ssrc != newSsrc) {
        m_ssrc = newSsrc;
        emit ssrcChanged();
    }

}

// Reset the SSRC to its default value
void WebRTC::resetSsrc()
{
    m_ssrc = 2; // Assuming 2 is the default SSRC value
    emit ssrcChanged();
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
    emit isOffererChanged();
}


