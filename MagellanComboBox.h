#ifndef MAGELLANCOMBOBOX_H
#define MAGELLANCOMBOBOX_H


#include <QComboBox>
#include <QKeyEvent>
#include <QCompleter>
#include <QDebug>
#include <QAbstractItemView>

class MagellanComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit MagellanComboBox(QWidget *parent = nullptr) : QComboBox(parent) {}

signals:
    void returnPressed(const QString &firstSuggestion);

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        QComboBox::keyPressEvent(event); // 让基类处理其他按键（如上下箭头）

        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            // 获取 completer 的 popup 列表中的第一项
            QCompleter *completer = this->completer();
            if (completer && completer->popup()->isVisible() && completer->completionCount() > 0) {
                QModelIndex firstIndex = completer->completionModel()->index(0, 0);
                if (firstIndex.isValid()) {
                    QString firstText = completer->completionModel()->data(firstIndex, Qt::UserRole).toString();
                    emit returnPressed(firstText);
                    return;
                }
            }

            // 如果没有补全项，则发送当前输入框的内容
            emit returnPressed(this->currentText());
        }
    }
};


#endif // MAGELLANCOMBOBOX_H
