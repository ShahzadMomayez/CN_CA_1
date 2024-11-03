#include "webrtc.h"
#include <QtEndian>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#define D qDebug() << Q_FUNC_INFO
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

        // m_localDescription = descriptionToJson(m_peerConnections[peerID]->localDescription().value());
        // Q_EMIT localDescriptionGenerated(peerID, m_localDescription);
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

    m_config.iceServers.emplace_back("stun.l.google.com:19302");

    m_audio = rtc::Description::Audio();
    m_audio.addOpusCodec(m_payloadType);
    m_audio.addSSRC(m_ssrc, "voice");
    if (m_isOfferer) {
        m_audio.setDirection(rtc::Description::Direction::SendOnly);
    } else {
        m_audio.setDirection(rtc::Description::Direction::RecvOnly);
    }

    m_peerConnections.clear();
    m_peerSdps.clear();
    m_peerTracks.clear();
}


void WebRTC::addPeer(const QString &peerId)
{
    auto newPeer = std::make_shared<rtc::PeerConnection>(m_config);
    m_peerConnections.insert(peerId, newPeer);

    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        QString sdp = QString::fromStdString(description.generateSdp());
        emit localDescriptionGenerated(peerId, sdp);
        auto peer  = m_peerConnections[peerId];
        m_localDescription = sdp;

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

    newPeer->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        switch (state) {
        case rtc::PeerConnection::State::New:
            qDebug() << "Peer" << peerId << "is in state New.";
            break;
        case rtc::PeerConnection::State::Connecting:
            break;
        case rtc::PeerConnection::State::Connected:
            // emit openedDataChannel(peerId);
            break;
        case rtc::PeerConnection::State::Disconnected:
            emit closedDataChannel(peerId);
            break;
        case rtc::PeerConnection::State::Failed:
            emit closedDataChannel(peerId);
            break;
        case rtc::PeerConnection::State::Closed:
            emit closedDataChannel(peerId);
            break;
        }
    });

    newPeer->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            D<<"gathering StateChange";
            auto peer = m_peerConnections[peerId];
            if (!m_isOfferer)
            {
                D<<"!m_isOfferer";
            }
            QString sdpJson = this->descriptionToJson(peer->localDescription().value());
            if(!m_trrakOpen)
                emit sdpGenerated(sdpJson);
            D<<"Emited";
            emit gatheringComplited(peerId);

        }
    });
    newPeer->onTrack([this, peerId](std::shared_ptr<rtc::Track> track) {
        D<<"track is open";
    });

    this->addAudioTrack(peerId,"shahzad");
}


void WebRTC::generateOfferSDP(const QString &peerId)
{
    auto peerConnection = m_peerConnections.value(peerId);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerId;
        return;
    }
    peerConnection->setLocalDescription(rtc::Description::Type::Offer);
}

void WebRTC::generateAnswerSDP(const QString &peerId) {
    auto peerConnection = m_peerConnections.value(peerId);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerId;
        return;
    }
    peerConnection->setLocalDescription(rtc::Description::Type::Answer);
    peerConnection->localDescription()->generateSdp();
    D<<"generateAnswerSDP";
}

void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName) {
    auto peerConnection = m_peerConnections.value(peerId);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerId;
        return;
    }

    auto audioTrack = peerConnection->addTrack(m_audio);
    m_peerTracks.insert(peerId, audioTrack);
    audioTrack->onOpen([this]()
                       {
                  D<<"trak is open";
        m_trrakOpen = true;
        emit trackIsOPen();
    });
    audioTrack->onMessage(
        [this, peerId](rtc::message_variant data) {
            QByteArray recievedData= readVariant(data);

            emit packetRecieved(recievedData);

        }
        );
    qDebug() << "Audio track added for peer:" << peerId;
}

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

    RtpHeader header;
    header.ssrc = qToBigEndian(m_ssrc);
    header.first = 0x80;
    header.marker = 0;
    header.payloadType = m_payloadType;
    header.sequenceNumber = qToBigEndian(++m_sequenceNumber);
    header.timestamp = qToBigEndian(getCurrentTimestamp());

    QByteArray packet(reinterpret_cast<const char*>(&header), sizeof(RtpHeader));
    packet.append(buffer);

    std::vector<std::byte> packetBytes(packet.size());
    std::memcpy(packetBytes.data(), packet.data(), packet.size());

    audioTrack->send(rtc::binary(packetBytes));
    qDebug() << "Audio data sent to peer:" << peerId;
}




/**
 * ====================================================
 * ================= public slots =====================
 * ====================================================
 */

void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{

    D<<"peer: " << peerID
      << "Remote description: "
      << "sdp: " << sdp;

    m_remoteDescription = sdp.toUtf8();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(sdp.toUtf8());
    D<<jsonDoc["sdp"]<<"log";
    rtc::Description description(jsonDoc["sdp"].toString().toStdString(),
                                 jsonDoc["type"].toString().toStdString());

    if(!m_trrakOpen)
        m_peerConnections[peerID]->setRemoteDescription(description);
}


void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{
    auto peerConnection = m_peerConnections.value(peerID);
    if (!peerConnection) {
        qWarning() << "Peer connection not found for" << peerID;
        return;
    }

    if (candidate.isEmpty() || sdpMid.isEmpty()) {
        qWarning() << "Invalid candidate or sdpMid.";
        return;
    }

    std::string candidateStr = candidate.toStdString();
    std::string sdpMidStr = sdpMid.toStdString();

    try {
        if (!peerConnection->remoteDescription()) {
            qWarning() << "Remote description must be set before adding ICE candidates.";
            return;
        }

        rtc::Candidate iceCandidate(candidateStr, sdpMidStr);
        peerConnection->addRemoteCandidate(iceCandidate);
        qDebug() << "Remote candidate added for peer:" << peerID;
    } catch (const std::exception &e) {
        qWarning() << "Failed to add remote candidate: " << QString::fromStdString(std::string(e.what()));

    }
}



/*
 * ====================================================
 * ================= private methods ==================
 * ====================================================
 */

QByteArray WebRTC::readVariant(const rtc::message_variant &data)
{
    QByteArray byteArray;

    if (std::holds_alternative<rtc::binary>(data)) {
        const rtc::binary &binaryData = std::get<rtc::binary>(data);
        byteArray = QByteArray(reinterpret_cast<const char*>(binaryData.data() + 12), static_cast<int>(binaryData.size()-12));
    } else if (std::holds_alternative<std::string>(data)) {
        const std::string &stringData = std::get<std::string>(data);
        byteArray = QByteArray::fromStdString(stringData);
    } else {
        qWarning() << "Unknown variant type.";
    }

    return byteArray;
}

QString WebRTC::descriptionToJson(const rtc::Description &description)
{

    QJsonObject json;
    json["type"] = (m_isOfferer)? "offer": "answer" ;
    json["sdp"] = QString::fromStdString(description.generateSdp());
    QJsonDocument doc(json);
    return QString(doc.toJson());
}

int WebRTC::bitRate() const
{
    return m_bitRate;
}

void WebRTC::setBitRate(int newBitRate)
{
    if (m_bitRate != newBitRate) {
        m_bitRate = newBitRate;
        emit bitRateChanged();
    }
}

void WebRTC::resetBitRate()
{
    m_bitRate = 48000;
    emit bitRateChanged();
}

void WebRTC::setPayloadType(int newPayloadType)
{
    if (m_payloadType != newPayloadType) {
        m_payloadType = newPayloadType;
        emit payloadTypeChanged();
    }
}

void WebRTC::resetPayloadType()
{
    m_payloadType = 111;
    emit payloadTypeChanged();
}

rtc::SSRC WebRTC::ssrc() const
{
    return m_ssrc;
}

void WebRTC::setSsrc(rtc::SSRC newSsrc)
{
    if (m_ssrc != newSsrc) {
        m_ssrc = newSsrc;
        emit ssrcChanged();
    }

}

void WebRTC::resetSsrc()
{
    m_ssrc = 2;
    emit ssrcChanged();
}

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


