//
// Created by John on 14.10.2019.
//


#include "QPenDialog.hpp"


QPenDialog::QPenDialog(QWidget *parent) : QDialog(parent), ui(new Ui::uiQPenDialog) {
	ui->setupUi(this);

	penColorDialog->setOption(QColorDialog::NoButtons);
	penColorDialog->setOption(QColorDialog::ShowAlphaChannel);
	penColorDialog->setWindowFlags(Qt::Widget);
	penColorDialog->setSizeGripEnabled(false);
	QGridLayout *gridLayoutColor = new QGridLayout(ui->widget_penColor);
	gridLayoutColor->addWidget(penColorDialog);

	ui->comboBox_penStyle->addItem(tr("Solid"), static_cast<int>(Qt::SolidLine));
	ui->comboBox_penStyle->addItem(tr("Dash"), static_cast<int>(Qt::DashLine));
	ui->comboBox_penStyle->addItem(tr("Dot"), static_cast<int>(Qt::DotLine));
	ui->comboBox_penStyle->addItem(tr("Dash Dot"), static_cast<int>(Qt::DashDotLine));
	ui->comboBox_penStyle->addItem(tr("Dash Dot Dot"), static_cast<int>(Qt::DashDotDotLine));
	ui->comboBox_penStyle->addItem(tr("None"), static_cast<int>(Qt::NoPen));

	ui->comboBox_penCap->addItem(tr("Flat"), Qt::FlatCap);
	ui->comboBox_penCap->addItem(tr("Square"), Qt::SquareCap);
	ui->comboBox_penCap->addItem(tr("Round"), Qt::RoundCap);

	ui->comboBox_penJoin->addItem(tr("Miter"), Qt::MiterJoin);
	ui->comboBox_penJoin->addItem(tr("Bevel"), Qt::BevelJoin);
	ui->comboBox_penJoin->addItem(tr("Round"), Qt::RoundJoin);

	connect(ui->spinBox_penWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &QPenDialog::penChanged);
	connect(penColorDialog, &QColorDialog::currentColorChanged, this, &QPenDialog::penChanged);
	connect(ui->comboBox_penStyle, QOverload<int>::of(&QComboBox::activated), this, &QPenDialog::penChanged);
	connect(ui->comboBox_penCap, QOverload<int>::of(&QComboBox::activated), this, &QPenDialog::penChanged);
	connect(ui->comboBox_penJoin, QOverload<int>::of(&QComboBox::activated), this, &QPenDialog::penChanged);

	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [=]() {
		emit penSelected(*pen);
		close();
	}); // clicked ok

	penChanged();
}

QPenDialog::~QPenDialog() {
	delete ui;
	delete pen;
	delete penColorDialog;
}

void QPenDialog::setCurrentPen(const QPen &apen) {
	// update widgets accordingly
	ui->spinBox_penWidth->setValue(apen.width());
	penColorDialog->setCurrentColor(apen.color());
	ui->comboBox_penStyle->setCurrentIndex(ui->comboBox_penStyle->findData(static_cast<int>(apen.style())));
	ui->comboBox_penCap->setCurrentIndex(ui->comboBox_penCap->findData(apen.capStyle()));
	ui->comboBox_penJoin->setCurrentIndex(ui->comboBox_penJoin->findData(apen.capStyle()));

	penChanged();
}

QPen QPenDialog::currentPen() const {
	return *pen;
}

void QPenDialog::penChanged() {
	int width = ui->spinBox_penWidth->value();
	QColor color = penColorDialog->currentColor();
	Qt::PenStyle style = Qt::PenStyle(ui->comboBox_penStyle->itemData(ui->comboBox_penStyle->currentIndex(), Qt::UserRole).toInt());
	Qt::PenCapStyle cap = Qt::PenCapStyle(ui->comboBox_penCap->itemData(ui->comboBox_penCap->currentIndex(), Qt::UserRole).toInt());
	Qt::PenJoinStyle join = Qt::PenJoinStyle(ui->comboBox_penJoin->itemData(ui->comboBox_penJoin->currentIndex(), Qt::UserRole).toInt());

	pen->setWidth(width);
	pen->setColor(color);
	pen->setStyle(style);
	pen->setCapStyle(cap);
	pen->setJoinStyle(join);

	emit currentPenChanged(*pen);
}


