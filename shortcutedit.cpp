#include <QKeyEvent>
#include <QSettings>
#include "shortcutedit.h"

ShortcutEdit::ShortcutEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

ShortcutEdit::~ShortcutEdit()
{

}

/**
 * @brief ShortcutEdit::setKey Set key sequence.
 * @param key Key sequence.
 */
void ShortcutEdit::setKey(const QKeySequence &key)
{
    if (!key.isEmpty()) {
        m_key = key;
        this->setText(m_key.toString(QKeySequence::NativeText));
    }
}

/**
 * @brief ShortcutEdit::getKey Get key sequence.
 * @return Key sequence.
 */
QKeySequence ShortcutEdit::getKey() const
{
    return m_key;
}

void ShortcutEdit::keyPressEvent(QKeyEvent *event)
{
    int key;
    Qt::KeyboardModifiers modifiers;

    // get key and modifiers
    key = event->key();
    modifiers = event->modifiers() & ~Qt::KeypadModifier;

    // modify key and modifiers
    switch (key) {
    case Qt::Key_unknown: // unknow
    case Qt::Key_AltGr:
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Meta:
    case Qt::Key_Alt:
        key = Qt::Key_No;
        break;

    default:
        break;
    }

    // build key sequence
    if (key != Qt::Key_No && modifiers) {
        m_key = QKeySequence(key | modifiers);
        this->setText(m_key.toString(QKeySequence::NativeText));
        emit shortcutChanged(m_key);
    }
}
