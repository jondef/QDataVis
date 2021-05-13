//
// Created by TAADEJOM on 13.05.2021.
//

#ifndef QDATAVIS_QTEXTEDITCUSTOM_HPP
#define QDATAVIS_QTEXTEDITCUSTOM_HPP

#include <QKeyEvent>
#include <QTextEdit>

class QTextEditCustom : public QTextEdit {
Q_OBJECT
    using QTextEdit::QTextEdit;

public:
    QTextEditCustom(QWidget *parent) : QTextEdit(parent) {
        setTabChangesFocus(true);
    }

protected:
    void keyPressEvent(QKeyEvent *ev) override;

signals:
    void inputAccepted();
};


#endif //QDATAVIS_QTEXTEDITCUSTOM_HPP
