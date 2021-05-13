//
// Created by TAADEJOM on 13.05.2021.
//

#include <QCoreApplication>
#include "QTextEditCustom.hpp"

void QTextEditCustom::keyPressEvent(QKeyEvent *ev) {
    // if user presses shift+enter, send an accept signal
    if (ev->key() == Qt::Key_Return && ev->modifiers() == Qt::ShiftModifier) {
        emit inputAccepted();
        clear();
        return;
    }

    // auto if user is trying to close parentheses and there is one ahead of the cursor already, simply move the cursor
    if (ev->text() == ")" && !textCursor().hasSelection()) {
        if (textCursor().position() < toPlainText().size() && toPlainText()[textCursor().position()] == ')') {
            moveCursor(QTextCursor::Right);
            return;
        }
    }
    // auto close parentheses
    if (ev->key() == Qt::Key_Backspace) {
        if (textCursor().position() < toPlainText().size() && toPlainText()[textCursor().position()] == ')') {
            if (textCursor().position() > 0 && toPlainText()[textCursor().position() - 1] == '(') {
                QKeyEvent event(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
                QCoreApplication::sendEvent(this, &event);
            }
        }
    }
    // if user opened a parentheses then close it directly
    if (ev->text() == "(") {
        insertPlainText(")");
        moveCursor(QTextCursor::Left);
    }
    QTextEdit::keyPressEvent(ev);
}
