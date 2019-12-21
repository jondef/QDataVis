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
		emit currentGraphChanged();
		this->close();
	});

	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [this]() {
		this->close();
	});
	//line style
	ui->comboBox_lineStyle->addItem("None", QVariant(QCPGraph::lsNone));
	ui->comboBox_lineStyle->addItem("Line", QVariant(QCPGraph::lsLine));
	ui->comboBox_lineStyle->addItem("Step Left", QVariant(QCPGraph::lsStepLeft));
	ui->comboBox_lineStyle->addItem("Step Right", QVariant(QCPGraph::lsStepRight));
	ui->comboBox_lineStyle->addItem("Step Center", QVariant(QCPGraph::lsStepCenter));
	ui->comboBox_lineStyle->addItem("Impulse", QVariant(QCPGraph::lsImpulse));

	//Scatter style
	ui->comboBox_scatterStyle->addItem("None", QVariant(QCPScatterStyle::ssNone));
	ui->comboBox_scatterStyle->addItem("Dot", QVariant(QCPScatterStyle::ssDot));
	ui->comboBox_scatterStyle->addItem("Cross", QVariant(QCPScatterStyle::ssCross));
	ui->comboBox_scatterStyle->addItem("Plus", QVariant(QCPScatterStyle::ssPlus));
	ui->comboBox_scatterStyle->addItem("Circle", QVariant(QCPScatterStyle::ssCircle));
	ui->comboBox_scatterStyle->addItem("Disc", QVariant(QCPScatterStyle::ssDisc));
	ui->comboBox_scatterStyle->addItem("Square", QVariant(QCPScatterStyle::ssSquare));
	ui->comboBox_scatterStyle->addItem("Diamond", QVariant(QCPScatterStyle::ssDiamond));
	ui->comboBox_scatterStyle->addItem("Star", QVariant(QCPScatterStyle::ssStar));
	ui->comboBox_scatterStyle->addItem("Triangle", QVariant(QCPScatterStyle::ssTriangle));
	ui->comboBox_scatterStyle->addItem("Inverted Triangle", QVariant(QCPScatterStyle::ssTriangleInverted));
	ui->comboBox_scatterStyle->addItem("Cross Square", QVariant(QCPScatterStyle::ssCrossSquare));
	ui->comboBox_scatterStyle->addItem("Plus Square", QVariant(QCPScatterStyle::ssPlusSquare));
	ui->comboBox_scatterStyle->addItem("Cross Circle", QVariant(QCPScatterStyle::ssCrossCircle));
	ui->comboBox_scatterStyle->addItem("Plus Circle", QVariant(QCPScatterStyle::ssPlusCircle));
	ui->comboBox_scatterStyle->addItem("Peace", QVariant(QCPScatterStyle::ssPeace));
	ui->comboBox_scatterStyle->addItem("Pixmap", QVariant(QCPScatterStyle::ssPixmap));
	ui->comboBox_scatterStyle->addItem("Custom", QVariant(QCPScatterStyle::ssCustom));


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

	//Set graph linestyle
	m_graph->setLineStyle(static_cast<QCPGraph::LineStyle>(ui->comboBox_lineStyle->currentData().toInt()));

	//Set graph scatterstyle
	m_graph->setScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(ui->comboBox_scatterStyle->currentData().toInt()));
}

void PointWindow::setGraph(QListWidgetItem *listWidgetItem, QCPGraph *const pGraph) {
	m_listWidgetItem = listWidgetItem;
	m_graph = pGraph;

	//set graphpoints in pointdata textedit
	ui->textEdit_graphPoints->clear();
	for (QCPDataContainer<QCPGraphData>::iterator i = pGraph->data()->begin(); i != pGraph->data()->end(); ++i) {
		ui->textEdit_graphPoints->append(QString("%1, %2").arg(i->key).arg(i->value));
	}

	//set graph Name
	ui->lineEdit_graphName->setText(listWidgetItem->text());

	//Set line style
	ui->comboBox_lineStyle->setCurrentIndex(ui->comboBox_lineStyle->findData(static_cast<int>(m_graph->lineStyle())));

	//set scatter style
	ui->comboBox_scatterStyle->setCurrentIndex(ui->comboBox_scatterStyle->findData(static_cast<int>(m_graph->scatterStyle().shape())));
}


