#ifndef LED_H
#define LED_H

#include <QObject>

class QString;

class Led : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool On READ Get WRITE Set NOTIFY OnChanged)

public:
    explicit Led(const QString &ledPath, QObject *parent = nullptr);
    ~Led();

    bool isValid() const;

public slots:
    bool Set(bool On);
    bool Get() const;
    bool Toggle();

signals: 
    void OnChanged(bool Status);

private:
    int m_fd = -1;

    bool m_isValid = false;
};

#endif