#ifndef MORSETRANSMITTER_H
#define MORSETRANSMITTER_H

#include <QObject>

#include <QString>
#include <QMap>

#include "dbusled.h"

class MorseTransmitter : public QObject 
{
    Q_OBJECT

public:
    MorseTransmitter(QObject *parent = nullptr);

    void transmitText(const QString &text);

    void setUnitDuration(int duration);

private slots:
    void sendNextSymbol();

private:
    su::powar::LedDaemon *m_ledInterface;
    QString m_morseString;
    int m_currentPos = 0;
    int m_unitDuration = 200;

    static const QMap<QChar, QString> s_morseCodeMap;
    static const QMap<QChar, QString> s_specialMorseCodeMap;
};

#endif