#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QDebug>

// Including necessary headers
#include "audioinput.h"
#include "audiooutput.h"
#include "uihandler.h"
#include "webrtc.h"
//#include "signalingserver.h" // Uncomment if SignalingServer is used

int main(int argc, char *argv[])
{
    qDebug()<< "start main ....";
    QGuiApplication app(argc, argv); // Use QGuiApplication instead of QCoreApplication

    QQmlApplicationEngine engine;
    qmlRegisterType<UIHandler>("UIHandler", 1, 0, "UIHandler");

    QUrl url("qrc:/main.qml");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::AutoConnection);
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;


    return app.exec();
}
