#ifndef HOTKEYHANDLER_H
#define HOTKEYHANDLER_H

#include <QAbstractNativeEventFilter>
#include <QObject>

class HotkeyHandler : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    HotkeyHandler();

    ~HotkeyHandler();

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
    bool registerHotkey();
    void unregisterHotkey();

signals:
    void hotKeyPressed();

private:
    quint32 m_hotKeyId = 1;
};

#endif // HOTKEYHANDLER_H
