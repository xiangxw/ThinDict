#ifndef SHORTCUTEDIT_H
#define SHORTCUTEDIT_H

#include <QLineEdit>
#include <QKeySequence>

class ShortcutEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit ShortcutEdit(QWidget *parent = 0);
    ~ShortcutEdit();

    void setKey(const QKeySequence &key);
    QKeySequence getKey() const;

signals:
    void shortcutChanged(const QKeySequence &key);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    QKeySequence m_key;
};

#endif // SHORTCUTEDIT_H
