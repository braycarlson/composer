#include <QApplication>
#include <QCoreApplication>
#include <QtDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QStyleFactory>
#include <QTextStream>

#include "include/window.h"


void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    Q_UNUSED(context);

    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", localMsg.constData());
        fflush(stderr);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s\n", localMsg.constData());
        fflush(stderr);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", localMsg.constData());
        fflush(stderr);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", localMsg.constData());
        fflush(stderr);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", localMsg.constData());
        fflush(stderr);
        abort();
    }
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.addLibraryPath("plugins");
    app.addLibraryPath("plugins/mediaservice");

    QDir().mkpath("binary");
    QDir().mkpath("icon");
    QDir().mkpath("music");
    QDir().mkpath("playlist");

    qInstallMessageHandler(messageHandler);

    Window window;
    window.show();
    window.setFocus();

    QString stylesheet = "src/stylesheet.qss";

    QFile file(stylesheet);

    if (file.open(QFile::ReadOnly)) {
        QString stylesheet = QTextStream(&file).readAll();
        app.setStyleSheet(stylesheet);
    }

    app.setStyle(QStyleFactory::create("Fusion"));

    window.onRosePineThemeApplied();

    return app.exec();
}
