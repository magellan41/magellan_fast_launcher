#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDebug>

#include <QScreen>

#include <QCompleter>
#include <QLineEdit>
#include <QStandardItemModel >
#include <QVBoxLayout>

#include <QCoreApplication>
#include <QDir>

#include <QDragEnterEvent>
#include <QMimeData>

#include <QMessageBox>

#include <QDesktopServices>

#include <QSystemTrayIcon>

#include <QSettings>

#include "MagellanComboBox.h"
#include "hotkeyhandler.h"
#include "magellan_fast_launcher_sqllite.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onHotKeyPressed();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::MainWindow *ui;
    QString source_path;
    QString auto_start_key = "MagellanFastLauncher";
    LnkDB db;

    QSystemTrayIcon *m_trayIcon;
    class HotkeyHandler *m_hotkeyHandler;

    bool isAutoStartEnabled();
    void enableAutoStart(bool enable);
};
#endif // MAINWINDOW_H
