#include <QCoreApplication>
#include <QCommandLineParser>
#include "led.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral(PROJECT_NAME));

    if (qgetenv("USER") != QLatin1String("root")) {
        qCritical() << "The daemon must be ran as root";
        return EXIT_FAILURE;
    }

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("The D-Bus daemon for a led control"));
    parser.addHelpOption();

    parser.addPositionalArgument(QStringLiteral("led"), QStringLiteral("The led name in /sys/class/leds."));

    parser.process(app);

    QStringList args = parser.positionalArguments();
    if (args.count() != 1) {
        parser.showHelp(EXIT_FAILURE);
    }
    Led led(args.at(0));
    if (!led.isValid()) {
        qCritical() << "Cannot open the led" << args.at(0);
        return EXIT_FAILURE;
    }

    qInfo() << "Daemon started at su.powar.LedDaemon on the system bus";

    return app.exec();
}