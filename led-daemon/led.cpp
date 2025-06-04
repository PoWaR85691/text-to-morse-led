#include "led.h"

#include <QDir>

#include "leddaemonadaptor.h"

#include <fcntl.h>
#include <qlogging.h>
#include <unistd.h>

static const QString LED_DIRECTORY = QStringLiteral("/sys/class/leds");
static const QString LED_BRIGHTNESS = QStringLiteral("brightness");

Led::Led(const QString &ledName, QObject *parent)
    : QObject(parent)
{
    m_fd = open(QDir(QDir(LED_DIRECTORY).absoluteFilePath(ledName)).absoluteFilePath(LED_BRIGHTNESS).toLocal8Bit().data(), 
                O_RDWR);
    if (m_fd <= 0) {
        qCritical() << "Cannot open led file";
        return;
    }

    if (!QDBusConnection::systemBus().registerService(
            QStringLiteral("su.powar.LedDaemon"))) {
        qCritical() << "Cannot register service";
        return;
    }
    if (!QDBusConnection::systemBus().registerObject(
            QStringLiteral("/su/powar/LedDaemon"), this)) {
        qCritical() << "Cannot register object";
        return;
    }
    new LedDaemonAdaptor(this);

    m_isValid = true;
}

Led::~Led()
{
    close(m_fd);
}

bool Led::isValid() const
{
    return m_isValid;
}

bool Led::Set(bool On)
{
    bool changed = Get() != On;
    if (changed) {
        char value = On ? '1' : '0';
        lseek(m_fd, 0, SEEK_SET);
        if (write(m_fd, &value, 1) <= 0) {
            qCritical() << "Cannot set led mode";
            On = false;
        }
        emit OnChanged(On);
    }
    return On;
}

bool Led::Get() const
{
    char value = '0';
    lseek(m_fd, 0, SEEK_SET);
    if (read(m_fd, &value, 1) <= 0) {
        qCritical() << "Cannot get led mode";
    }
    return value != '0';
}

bool Led::Toggle()
{
    return Set(!Get());
}