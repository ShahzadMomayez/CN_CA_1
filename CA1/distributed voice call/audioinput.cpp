#include "audioinput.h"
#include <QDebug>

AudioInput::AudioInput(QObject *parent) : QIODevice(parent) {
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    audioSource = new QAudioSource(format, this);
    opusEncoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_AUDIO, &opusError);
    if (opusError != OPUS_OK) {
        qWarning("Failed to create Opus encoder: %s", opus_strerror(opusError));
    }
}

void AudioInput::start() {
    open(QIODevice::WriteOnly);
    audioSource->start(this);
    qDebug() << "Recording started...";
}

void AudioInput::stop() {
    audioSource->stop();
    close();
    qDebug() << "Recording stopped...";
}

qint64 AudioInput::writeData(const char *data, qint64 len) {
    if (opusEncoder == nullptr) {
        return -1;
    }

    opus_int16 inputBuffer[48000];
    int inputFrameSize = len / sizeof(opus_int16);

    if (inputFrameSize != 120 && inputFrameSize != 240 && inputFrameSize != 480 &&
        inputFrameSize != 960 && inputFrameSize != 1920 && inputFrameSize != 2880) {
        qWarning("Invalid frame size for Opus encoding: %d", inputFrameSize);
        return -1;
    }

    unsigned char outputBuffer[4000];
    int encodedBytes = opus_encode(opusEncoder, (const opus_int16 *)data, inputFrameSize, outputBuffer, sizeof(outputBuffer));

    if (encodedBytes < 0) {
        qWarning("Opus encoding failed: %s", opus_strerror(encodedBytes));
        return -1;
    }

    QByteArray encodedData(reinterpret_cast<char *>(outputBuffer), encodedBytes);
    emit newDataAvailable(encodedData);

    return len;

}
