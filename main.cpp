#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

// Get the default Qt message handler.
// static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

void myCustomMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt;
    switch (type) {
        case QtDebugMsg:
            txt = QString("Debug: %1").arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("Warning: %1").arg(msg);
            break;
        case QtCriticalMsg:
            txt = QString("Critical: %1").arg(msg);
            break;
        case QtFatalMsg:
        default:
            txt = QString("Fatal: %1").arg(msg);
            abort();
    }
    QString path = QDir::homePath();
    path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);//, ".cp-camera-face", QStandardPaths::LocateDirectory);
    path.append("/.cp-camera-face");
    if (!QDir(path).exists()) {
        QDir().mkdir(path);
    }
    //qDebug() << "App path : " << path;
    path.append("/cp-camera-face.log");
    //qDebug() << "log path: " << path;
    QFile outFile(path);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(myCustomMessageHandler);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "cp-camera-face_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
