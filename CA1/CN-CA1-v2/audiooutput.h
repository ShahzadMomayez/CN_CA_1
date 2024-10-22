#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>
#include <QMutex>
#include <QAudioSink>
#include <QAudioFormat>
#include <QIODevice>
#include "opus.h"

class AudioOutput : public QObject {
    Q_OBJECT

public:
    AudioOutput(QObject *parent = nullptr);
    void start();
    void stop();

public slots:
    void addData(const QByteArray &data);

private:
    QIODevice *audioDevice;
    QAudioSink *audioSink;
    OpusDecoder *opusDecoder;
    QMutex mutex;
    int opusError;
};

#endif // AUDIOOUTPUT_H
