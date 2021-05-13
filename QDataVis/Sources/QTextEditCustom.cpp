//
// Created by TAADEJOM on 13.05.2021.
//

#include <QCoreApplication>
#include "QTextEditCustom.hpp"
#include "QCustomPlotCustom.hpp"
#include "Node.hpp"


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
    highlightParentheses();
}

void QTextEditCustom::highlightParentheses() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    QList<int> pArray = Node::getParenthesesArray(toPlainText());
    for (int i = 0; i < toPlainText().size(); ++i) {
        if (toPlainText()[i] == '(' || toPlainText()[i] == ')') {
            QTextEdit::ExtraSelection selection;
            selection.format.setForeground(QCustomPlotCustom::getGraphColor(pArray.at(i)));
            selection.format.setBackground(QCustomPlotCustom::getGraphColor(pArray.at(i)).darker(300));
            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            selection.cursor.setPosition(i);
            selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            extraSelections.append(selection);
        }
    }
    setExtraSelections(extraSelections);
}
