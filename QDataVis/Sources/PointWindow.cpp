//
// Created by John on 30.11.2019.
//

#include "PointWindow.hpp"


PointWindow::PointWindow(QWidget *parent) : QDialog(parent), ui(new Ui::uiPointWindow) {
	ui->setupUi(this);

	setFixedSize(width(), height());
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

	connect(ui->pushButton_setColor, &QPushButton::clicked, this, [this]() {
		pDataSet->changeColor(QColorDialog::getColor(pDataSet->color));
	});
}

PointWindow::~PointWindow() {
	delete ui;
	delete pDataSet;
	delete m_listWidgetItem;
}

void PointWindow::saveGraph() const {
	// save point data
	QStringList pointString = ui->textEdit_graphPoints->toPlainText().split('\n', Qt::SkipEmptyParts);
	QVector<double> xArray(pointString.size());
	QVector<double> yArray(pointString.size());
	for (int lineNumber = 0; lineNumber < pointString.size(); ++lineNumber) {
		QStringList pointCoord = pointString.at(lineNumber).split(',');
		xArray.replace(lineNumber, pointCoord.at(0).toDouble());
		yArray.replace(lineNumber, pointCoord.at(1).toDouble());
	}
	pDataSet->graph->setData(xArray, yArray);
	// Save graph name
	m_listWidgetItem->setText(ui->lineEdit_graphName->text());
	// Set graph line style
	pDataSet->graph->setLineStyle(static_cast<QCPGraph::LineStyle>(ui->comboBox_lineStyle->currentData().toInt()));
	// Set graph scatter style
	pDataSet->graph->setScatterStyle(static_cast<QCPScatterStyle::ScatterShape>(ui->comboBox_scatterStyle->currentData().toInt()));
	// set override point domain
	pDataSet->overrideGlobalPointDensity = ui->groupBox_overridePointDomain->isChecked();
	// set point density
	pDataSet->pointDensity = ui->spinBox_setPointDensity->value();
	// set minimum domain
	pDataSet->minimumDomain = ui->spinBox_setGraphMinimumDomain->value();
	// set maximum domain
	pDataSet->maximumDomain = ui->spinBox_setGraphMaximumDomain->value();
	// recalculate the points of the graph
	if (pDataSet->binaryTree != nullptr && pDataSet->overrideGlobalPointDensity) {
		pDataSet->recalculatePoints();
	}
}

void PointWindow::setGraph(QListWidgetItem *listWidgetItem) {
	m_listWidgetItem = listWidgetItem;
	pDataSet = listWidgetItem->data(Qt::UserRole).value<DataSet *>();

	// set graphpoints in pointdata textedit
	ui->textEdit_graphPoints->clear();
	for (QCPDataContainer<QCPGraphData>::iterator i = pDataSet->graph->data()->begin(); i != pDataSet->graph->data()->end(); ++i) {
		ui->textEdit_graphPoints->append(QString("%1, %2").arg(i->key).arg(i->value));
	}
	// set graph Name
	ui->lineEdit_graphName->setText(listWidgetItem->text());
	// Set line style
	ui->comboBox_lineStyle->setCurrentIndex(ui->comboBox_lineStyle->findData(static_cast<int>(pDataSet->graph->lineStyle())));
	// set scatter style
	ui->comboBox_scatterStyle->setCurrentIndex(ui->comboBox_scatterStyle->findData(static_cast<int>(pDataSet->graph->scatterStyle().shape())));
	// set override point domain
	ui->groupBox_overridePointDomain->setChecked(pDataSet->overrideGlobalPointDensity);
	// set point density
	ui->spinBox_setPointDensity->setValue(pDataSet->pointDensity);
	// set minimum domain
	ui->spinBox_setGraphMinimumDomain->setValue(pDataSet->minimumDomain);
	// set maximum domain
	ui->spinBox_setGraphMaximumDomain->setValue(pDataSet->maximumDomain);
	// disable the override point density if data set is made up of points and not a function
	ui->groupBox_overridePointDomain->setDisabled(pDataSet->binaryTree == nullptr);
}


