#pragma once

#include <QIODevice>
#include <QAudioSource>
#include "opus.h"
#include <QAudioDevice>

class AudioInput : public QIODevice {
    Q_OBJECT

public:
    AudioInput(QObject *parent = nullptr);
    void start();
    void stop();

protected:
    qint64 writeData(const char *data, qint64 len) override;
    qint64 readData(char *data, qint64 len) override { return -1; }

Q_SIGNALS:
    void newDataAvailable(const QByteArray &data);

private:
    QAudioSource *audioSource;
    OpusEncoder *opusEncoder;
    int opusError;
};
