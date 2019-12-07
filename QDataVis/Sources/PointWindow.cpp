//
// Created by John on 30.11.2019.
//

#include "PointWindow.h"


PointWindow::PointWindow(QWidget *parent) : QDialog(parent), ui(new Ui::uiPointWindow) {
	ui->setupUi(this);

	setFixedSize(width(), height()); //for no maximaize
	setWindowModality(Qt::WindowModal);

	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
		saveGraph();
		this->close();
	});

	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [this]() {
		this->close();
	});
}

PointWindow::~PointWindow() {
	delete ui;
}

void PointWindow::saveGraph() {
	// save point data
	QVector<double> xArray;
	QVector<double> yArray;
	QStringList pointString = ui->textEdit_graphPoints->toPlainText().split('\n', QString::SkipEmptyParts);
	for (QString &line : pointString) {
		QStringList pointCoord = line.split(',');
		xArray.append(pointCoord.at(0).toDouble());
		yArray.append(pointCoord.at(1).toDouble());
	}
	m_graph->setData(xArray, yArray);

	//Save graph name
	m_listWidgetItem->setText(ui->lineEdit_graphName->text());

	emit currentGraphChanged();
}

void PointWindow::setGraph(QCPGraph *const pGraph, QListWidgetItem *listWidgetItem) {
	m_listWidgetItem = listWidgetItem;
	m_graph = pGraph;

	//set graphpoints in pointdata textedit
	ui->textEdit_graphPoints->clear();
	for (QCPDataContainer<QCPGraphData>::iterator i = pGraph->data()->begin(); i != pGraph->data()->end(); ++i) {
		ui->textEdit_graphPoints->append(QString("%1, %2").arg(i->key).arg(i->value));
	}

	//set graph Name
	ui->lineEdit_graphName->setText(listWidgetItem->text());

}


