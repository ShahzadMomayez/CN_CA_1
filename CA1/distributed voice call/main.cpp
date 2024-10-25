#include <QCoreApplication>
#include <QMediaDevices>
#include <QDebug>
#include "audioinput.h"
#include "audiooutput.h"
#include "webrtc.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);


    WebRTC webRTCInstance;
    webRTCInstance.init("xjgfck", true);

    // QAudioDevice devinfo = QMediaDevices::defaultAudioInput();
    // if (devinfo.isNull()) {
    //     qWarning() << "No default audio input found!";
    //     return -1;
    // }

    // AudioInput audioInput(devinfo);
    // AudioOutput audioOutput;


    // // Connect the audio input's newDataAvailable signal to audio output's addData slot
    // QObject::connect(&audioInput, &AudioInput::newDataAvailable, &audioOutput, &AudioOutput::addData);

    // // STARTING recording and playback
    // audioInput.start();
    // audioOutput.start();

    return app.exec();
}
