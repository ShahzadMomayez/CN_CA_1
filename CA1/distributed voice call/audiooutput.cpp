#include "audiooutput.h"
#include <QDebug>
#include <QtMultimedia>

AudioOutput::AudioOutput(QObject *parent) : QObject(parent) {
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    audioSink = new QAudioSink(format, this);
    // opus decoder
    opusDecoder = opus_decoder_create(48000, 1, &opusError);
    if (opusError != OPUS_OK) {
        qWarning("Failed to create Opus decoder: %s", opus_strerror(opusError));
    }

    audioDevice = audioSink->start();  // start audio output
}

void AudioOutput::start() {
    audioDevice = audioSink->start();
    qDebug()<<"started";
}

void AudioOutput::stop() {
    audioSink->stop();
    opus_decoder_destroy(opusDecoder);
}

void AudioOutput::addData(const QByteArray &data) {
    QMutexLocker locker(&mutex);

    opus_int16 decodedData[4096];  // audio bufer
    int decodedSamples = opus_decode(opusDecoder, (const unsigned char *)data.data(), data.size(), decodedData, sizeof(decodedData) / 2, 0);

    if (decodedSamples < 0) {
        qWarning("Opus decoding failed: %s", opus_strerror(decodedSamples));
        return;
    }

    QByteArray rawData(reinterpret_cast<char *>(decodedData), decodedSamples * sizeof(opus_int16));
    audioDevice->write(rawData);  // writing  decoded data to audio output
}
