//
// Created by John on 14.10.2019.
//

#ifndef GUI_APP_QPENDIALOG_H
#define GUI_APP_QPENDIALOG_H

#include <QtGui/QPen>
#include <QtWidgets/QDialog>
#include "ui_qpendialog.h"


class QPenDialog : public QDialog {
Q_OBJECT

public:
	Ui::Dialog *ui;

	explicit QPenDialog(QWidget *parent = nullptr);
	//explicit QPenDialog(const QPen &initial, QWidget *parent = nullptr);
	~QPenDialog();

	void setCurrentPen(const QPen &pen);
	QPen currentPen() const;

Q_SIGNALS:

	void currentPenChanged(const QPen &pen);

	void penSelected(const QPen &pen);
};


#endif //GUI_APP_QPENDIALOG_H
