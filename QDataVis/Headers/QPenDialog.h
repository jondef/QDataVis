//
// Created by John on 14.10.2019.
//

#ifndef GUI_APP_QPENDIALOG_H
#define GUI_APP_QPENDIALOG_H

#include <QtGui>
#include <QtCore>
#include <QtWidgets>
#include "ui_QPenDialog.h"


class QPenDialog : public QDialog {
Q_OBJECT

public:
	Ui::uiQPenDialog *ui;

	explicit QPenDialog(QWidget *parent = nullptr);

	explicit QPenDialog(const QPen &initial, QWidget *parent = nullptr);

	~QPenDialog();

	void setCurrentPen(const QPen &apen);

	QPen currentPen() const;

private:
	QPen *pen = new QPen;

	QColorDialog *penColorDialog = new QColorDialog();

private slots:

	void penChanged();

Q_SIGNALS:

	void currentPenChanged(const QPen &apen);

	void penSelected(const QPen &apen);
};


#endif //GUI_APP_QPENDIALOG_H
