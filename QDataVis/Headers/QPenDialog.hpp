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

	explicit QPenDialog(QWidget *parent = nullptr);

	explicit QPenDialog(const QPen &initial, QWidget *parent = nullptr);

	~QPenDialog() override;

public:

	void setCurrentPen(const QPen &apen);

	QPen currentPen() const;

private slots:

	void penChanged();

Q_SIGNALS:

	void currentPenChanged(const QPen &apen);

	void penSelected(const QPen &apen);

private:
	Ui::uiQPenDialog *ui;

	QPen *pen = new QPen;

	QColorDialog *penColorDialog = new QColorDialog();
};


#endif //GUI_APP_QPENDIALOG_H
