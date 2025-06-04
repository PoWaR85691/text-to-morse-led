#include <QCoreApplication>

#include <morsetransmitter.h>

#include <QString>

#include <QCommandLineParser>

#include <QDebug>

int main(int argc, char *argv[]) 
{
    QCoreApplication app(argc, argv);
    
    app.setApplicationName(QStringLiteral(PROJECT_NAME));
    
    QCommandLineParser parser;
    parser.setApplicationDescription("Utility to translate text to morse code by led LED");
    parser.addHelpOption();
    
    parser.addPositionalArgument("text", 
        QStringLiteral("The text for translattion"));
    
    QCommandLineOption durationOption(
        QStringList() << "d" << "duration",
        QStringLiteral("Duration of tick in milliseconds (default 200)"),
        QStringLiteral("milliseconds")
    );
    parser.addOption(durationOption);
    
    parser.process(app);
    
    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        qCritical() << "The text is not set";
        parser.showHelp(1);
    }
    
    QString text = args.join(" ");
    
    int unitDuration = 200;
    if (parser.isSet(durationOption)) {
        bool ok;
        int duration = parser.value(durationOption).toInt(&ok);
        if (ok && duration > 0) {
            unitDuration = duration;
        } else {
            qWarning() << "Uncorrect duration. Set to 200";
        }
    }
    
    MorseTransmitter transmitter;
    transmitter.setUnitDuration(unitDuration);
    transmitter.transmitText(text);

    return app.exec();
}