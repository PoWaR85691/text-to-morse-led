#include "morsetransmitter.h"

#include <QTimer>
#include <cstdlib>
#include <qcoreapplication.h>
#include <qnamespace.h>
#include <qobjectdefs.h>

const QMap<QChar, QString> MorseTransmitter::s_morseCodeMap = {
    {'A', QStringLiteral(".-")},    {'B', QStringLiteral("-...")},  {'C', QStringLiteral("-.-.")},  {'D', QStringLiteral("-..")},
    {'E', QStringLiteral(".")},     {'F', QStringLiteral("..-.")},  {'G', QStringLiteral("--.")},   {'H', QStringLiteral("....")},
    {'I', QStringLiteral("..")},    {'J', QStringLiteral(".---")},  {'K', QStringLiteral("-.-")},   {'L', QStringLiteral(".-..")},
    {'M', QStringLiteral("--")},    {'N', QStringLiteral("-.")},    {'O', QStringLiteral("---")},   {'P', QStringLiteral(".--.")},
    {'Q', QStringLiteral("--.-")},  {'R', QStringLiteral(".-.")},   {'S', QStringLiteral("...")},   {'T', QStringLiteral("-")},
    {'U', QStringLiteral("..-")},   {'V', QStringLiteral("...-")},  {'W', QStringLiteral(".--")},   {'X', QStringLiteral("-..-")},
    {'Y', QStringLiteral("-.--")},  {'Z', QStringLiteral("--..")},
    
    {u'А', QStringLiteral(".-")},    {u'Б', QStringLiteral("-...")},  {u'В', QStringLiteral(".--")},   {u'Г', QStringLiteral("--.")},
    {u'Д', QStringLiteral("-..")},   {u'Е', QStringLiteral(".")},     {u'Ж', QStringLiteral("...-")},  {u'З', QStringLiteral("--..")},
    {u'И', QStringLiteral("..")},    {u'Й', QStringLiteral(".---")},  {u'К', QStringLiteral("-.-")},   {u'Л', QStringLiteral(".-..")},
    {u'М', QStringLiteral("--")},    {u'Н', QStringLiteral("-.")},    {u'О', QStringLiteral("---")},   {u'П', QStringLiteral(".--.")},
    {u'Р', QStringLiteral(".-.")},   {u'С', QStringLiteral("...")},   {u'Т', QStringLiteral("-")},     {u'У', QStringLiteral("..-")},
    {u'Ф', QStringLiteral("..-.")},  {u'Х', QStringLiteral("....")},  {u'Ц', QStringLiteral("-.-.")},  {u'Ч', QStringLiteral("---.")},
    {u'Ш', QStringLiteral("----")},  {u'Щ', QStringLiteral("--.-")},  {u'Ъ', QStringLiteral(".--.-")}, {u'Ы', QStringLiteral("-.--")},
    {u'Ь', QStringLiteral("-..-")},  {u'Э', QStringLiteral("..-..")}, {u'Ю', QStringLiteral("..--")},  {u'Я', QStringLiteral(".-.-")},
    
    {'0', QStringLiteral("-----")}, {'1', QStringLiteral(".----")}, {'2', QStringLiteral("..---")}, {'3', QStringLiteral("...--")},
    {'4', QStringLiteral("....-")}, {'5', QStringLiteral(".....")}, {'6', QStringLiteral("-....")}, {'7', QStringLiteral("--...")},
    {'8', QStringLiteral("---..")}, {'9', QStringLiteral("----.")},
    
    {' ', QStringLiteral("/")}
};

const QMap<QChar, QString> MorseTransmitter::s_specialMorseCodeMap = {
    {'.', QStringLiteral("......")}, {'!', QStringLiteral("--..--")}, {',', QStringLiteral(".-.-.-")}, 
    {'?', QStringLiteral("..--..")}, {'-', QStringLiteral("-....-")}, {'/', QStringLiteral("-..-.")}
};

MorseTransmitter::MorseTransmitter(QObject *parent) 
    : QObject(parent),
    m_ledInterface{new su::powar::LedDaemon(
        "su.powar.LedDaemon",
        "/su/powar/LedDaemon",
        QDBusConnection::systemBus(),
        this
    )}
{
    if (!m_ledInterface->isValid()) {
        qCritical() << "Cannot connect to DBus interface";
        QMetaObject::invokeMethod(qApp, &QCoreApplication::exit, Qt::QueuedConnection, EXIT_FAILURE);
        return;
    }
}

void MorseTransmitter::transmitText(const QString &text) 
{
    QString upperText = text.toUpper();
    m_morseString.clear();

    for (const QChar &c : upperText) {
        if (s_morseCodeMap.contains(c)) {
            m_morseString += s_morseCodeMap[c] + " ";
        } else if (s_specialMorseCodeMap.contains(c)) {
            m_morseString += "/ " + s_specialMorseCodeMap[c] + " ";
        }
        else {
            qWarning() << "Unsupported char:" << c;
        }
    }

    m_morseString.removeLast(); // Last space
    qDebug() << "Morse code:" << m_morseString;

    m_ledInterface->Set(false);
    m_currentPos = 0;
    QTimer::singleShot(0, this, &MorseTransmitter::sendNextSymbol);
}

void MorseTransmitter::setUnitDuration(int duration) {
    if (duration > 0) {
        m_unitDuration = duration;
    }
}

void MorseTransmitter::sendNextSymbol() {
    if (m_currentPos >= m_morseString.length()) {
        m_ledInterface->Set(false);
        qDebug() << "End transmission";
        QCoreApplication::quit();
        return;
    }

    QChar c = m_morseString.at(m_currentPos);
    m_currentPos++;

    if (c == '.') {
        m_ledInterface->Set(true);
        QTimer::singleShot(1 * m_unitDuration, this, [this]() { m_ledInterface->Set(false); });
        QTimer::singleShot(2 * m_unitDuration, this, &MorseTransmitter::sendNextSymbol);
    } else if (c == '-') {
        m_ledInterface->Set(true);
        QTimer::singleShot(3 * m_unitDuration, this, [this]() { m_ledInterface->Set(false); });
        QTimer::singleShot(4 * m_unitDuration, this, &MorseTransmitter::sendNextSymbol);
    } else if (c == ' ') {
        // 1 before
        QTimer::singleShot(2 * m_unitDuration, this, &MorseTransmitter::sendNextSymbol);
    } else if (c == '/') {
        // 3 before and 2 after
        QTimer::singleShot(2 * m_unitDuration, this, &MorseTransmitter::sendNextSymbol);
    }
}