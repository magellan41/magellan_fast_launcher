// hotkeyhandler.cpp
#include "hotkeyhandler.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

HotkeyHandler::HotkeyHandler() = default;

HotkeyHandler::~HotkeyHandler()
{
    unregisterHotkey();
}

bool HotkeyHandler::registerHotkey()
{
#ifdef Q_OS_WIN
    if (RegisterHotKey(NULL, m_hotKeyId, MOD_ALT, 'R')) {
        qDebug() << "Alt+R 热键注册成功";
        return true;
    } else {
        qDebug() << "Alt+R 热键注册失败，错误代码:" << GetLastError();
        return false;
    }
#else
    return false;
#endif
}

void HotkeyHandler::unregisterHotkey()
{
#ifdef Q_OS_WIN
    UnregisterHotKey(NULL, m_hotKeyId);
#endif
}

bool HotkeyHandler::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

#ifdef Q_OS_WIN
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_HOTKEY) {
        if (msg->wParam == m_hotKeyId) {
            emit hotKeyPressed();
            return true;
        }
    }
#endif
    return false;
}
