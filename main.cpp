#include <exception>
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "ballsfield.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    qmlRegisterType<BallsField>("BallsField", 1, 0, "BallsModel");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    try {
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
			 &app, [url](QObject *obj, const QUrl &objUrl) {
	    if (!obj && url == objUrl)
		QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	engine.load(url);
    }  catch (std::exception& ex) {
	qDebug() << ex.what();
	return 1;
    }


    return app.exec();
}
