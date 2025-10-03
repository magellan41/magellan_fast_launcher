#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_trayIcon(nullptr)
    , m_hotkeyHandler(new HotkeyHandler())
{
    ui->setupUi(this);

    QIcon icon(":/icons/favicon.ico");
    this->setWindowIcon(icon);

    // 设置托盘图标
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip("Magellan 快捷启动器");

    QMenu *trayMenu = new QMenu(this);
    QAction *showAction = trayMenu->addAction("显示主窗口");
    QAction *autoStartAction = trayMenu->addAction("开机自启");
    autoStartAction->setCheckable(true);               // 可勾选
    autoStartAction->setChecked(isAutoStartEnabled()); // 当前状态
    QAction *quitAction = trayMenu->addAction("退出");

    connect(autoStartAction, &QAction::triggered, [this, autoStartAction]() {
        // qDebug() << "Auto start action triggered";
        qDebug() << autoStartAction->isChecked() << this->isAutoStartEnabled();
        bool shouldEnable = !autoStartAction->isChecked(); // 因为 triggered 在切换后才触发
        enableAutoStart(shouldEnable);
        autoStartAction->setChecked(shouldEnable); // 确保状态同步
    });

    // 连接托盘点击事件（双击托盘可唤醒）
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);

    // === 热键注册 ===
    qApp->installNativeEventFilter(m_hotkeyHandler);
    connect(m_hotkeyHandler, &HotkeyHandler::hotKeyPressed, this, &MainWindow::onHotKeyPressed);
    m_hotkeyHandler->registerHotkey();

    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();
    connect(showAction, &QAction::triggered, this, &MainWindow::onHotKeyPressed);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    this->setFixedSize(400, 150);
    // 禁用最大化按钮
    this->setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
    // this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    // 屏幕左下角
    QScreen *screen = QGuiApplication::primaryScreen(); // 获取主屏幕
    QRect screenGeometry = screen->geometry();
    int x = 10;                                                  // 最左边
    int y = screenGeometry.height() - 1.5 * this->height() - 10; // 底部
    this->move(x, y);

    // 输入框
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    MagellanComboBox *comboBox = new MagellanComboBox();
    comboBox->setFixedWidth(this->width() * 0.8);
    comboBox->setEditable(true);

    QStandardItemModel *completerModel = new QStandardItemModel(this);
    // 2. 创建 QCompleter，使用这个模型
    QCompleter *completer = new QCompleter(completerModel, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    comboBox->setCompleter(completer);

    layout->addWidget(comboBox);
    layout->setAlignment(comboBox, Qt::AlignCenter);

    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);

    this->source_path = QCoreApplication::applicationDirPath() + "/source";
    QDir dir;
    if (!dir.exists(this->source_path)) {
        if (!dir.mkpath(this->source_path)) {
            qWarning() << "无法创建目录:" << this->source_path;
            return;
        }
    }
    connect(comboBox, &QComboBox::editTextChanged, this, [=](const QString &text) {
        // qDebug() << "输入框内容正在变化:" << text;
        completerModel->clear();
        if (text.isEmpty()) {
            return;
        }
        QList<Lnk> lnk_list = this->db.select_lnk(text, true);
        qDebug() << lnk_list.size();
        for (const Lnk &lnk : lnk_list) {
            QStandardItem *item = new QStandardItem(lnk.getBaseName());
            item->setData(lnk.getName(), Qt::UserRole);
            completerModel->appendRow(item);
        }
    });

    connect(comboBox, &MagellanComboBox::returnPressed, this, [=](const QString &text) {
        // qDebug() << "回车 pressed，第一个补全建议或当前文本：" << text;
        bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(text));
        if (!success) {
            QString msg = "打开失败";
            if (QFile::remove(text)) {
                qDebug() << "快捷方式已删除";
                msg += ",快捷方式已删除";
            }
            if (this->db.delete_lnk(text)) {
                qDebug() << "数据库项已删除";
                msg += ",数据库项已删除";
            }
            QMessageBox::warning(this, "打开失败", msg);
        } else {
            this->db.access(text);
            this->hide();
        }
    });

    // sql
    this->db = LnkDB();
    if (!this->db.create_db()) {
        QMessageBox::warning(this, "错误", "数据库创建失败");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // 只接受包含本地文件的拖拽
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        foreach (const QUrl &url, mimeData->urls()) {
            QString filePath = url.toLocalFile(); // 转换为本地路径

            if (filePath.isEmpty())
                continue;

            QFileInfo fileInfo(filePath);
            // qDebug() << fileInfo.fileName();

            if (!fileInfo.exists()) {
                QMessageBox::warning(this, "错误", "文件不存在: " + filePath);
                continue;
            }

            QString targetPath = this->source_path + "/" + fileInfo.fileName();
            // qDebug() << filePath;
            // qDebug() << targetPath;

            if (fileInfo.suffix().toLower() == "lnk") {
                // 快捷方式，直接复制
                if (!QFile::copy(filePath, targetPath)) {
                    QMessageBox::warning(this, "失败", "复制快捷方式失败");
                } else {
                    int id = this->db.getTotalCount() + 1;
                    QString name = targetPath;
                    QString base_name = fileInfo.baseName();
                    int count = 0;
                    int status = 1;
                    Lnk lnk = Lnk(id, name, base_name, count, status);
                    if (!this->db.insert_lnk(lnk)) {
                        QMessageBox::warning(this, "失败", "写入数据库失败");
                    }
                    QList<Lnk> lnk_list = this->db.select_lnk();
                    for (const Lnk &lnk : lnk_list) {
                        qDebug() << lnk.getName();
                    }
                }
            } else {
                // 非link
                QMessageBox::warning(this, "失败", "请拖入一个快捷方式文件");
            }
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭窗口时，不退出，而是隐藏到托盘
    if (m_trayIcon->isSystemTrayAvailable()) {
        this->hide();
        event->ignore();
        m_trayIcon->show();
        m_trayIcon->setToolTip("Magellan 快捷启动器 (运行中)");
    } else {
        // 托盘不可用才允许退出
        QMainWindow::closeEvent(event);
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            // 最小化时隐藏窗口到托盘
            this->hide();
            event->accept();
            m_trayIcon->show();
            return;
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    // 双击托盘唤醒
    if (reason == QSystemTrayIcon::DoubleClick) {
        this->onHotKeyPressed();
    }
}

void MainWindow::onHotKeyPressed()
{
    qDebug() << "热键被按下！显示主窗口";

    // 显示主窗口
    this->show();
    this->raise();
    this->activateWindow();
}

// 检查开机自启是否已启用
bool MainWindow::isAutoStartEnabled()
{
    // 获取当前应用的注册表路径
    QString appName = QCoreApplication::applicationName();
    QString regKey = QString(
                         "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\%1")
                         .arg(appName);

    // 使用 QSettings 来访问注册表
    QSettings settings(regKey, QSettings::NativeFormat);
    // qDebug() << settings.contains(this->auto_start_key);
    return settings.contains(this->auto_start_key); // 如果注册表中有这个键，表示开机自启已启用
}

// 启用/禁用开机自启
void MainWindow::enableAutoStart(bool enable)
{
    QString appName = QCoreApplication::applicationName();
    QString appPath = QCoreApplication::applicationFilePath();

    // 获取当前应用的注册表路径
    QString regKey = QString(
                         "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\\%1")
                         .arg(appName);

    QSettings settings(regKey, QSettings::NativeFormat);
    if (enable) {
        // 设置开机自启，注册应用程序路径
        settings.setValue(this->auto_start_key, appPath);
    } else {
        // 删除开机自启项
        settings.remove(this->auto_start_key);
    }
}
