//
// Created by John on 14.10.2019.
//


#include "QPenDialog.h"


QPenDialog::QPenDialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog) {
	ui->setupUi(this);

	ui->comboBox_penColor->addItem(tr("Black"), static_cast<uint>(Qt::black));
	ui->comboBox_penColor->addItem(tr("White"), static_cast<uint>(Qt::white));
	ui->comboBox_penColor->addItem(tr("Dark Gray"), static_cast<int>(Qt::darkGray));
	ui->comboBox_penColor->addItem(tr("Gray"), static_cast<int>(Qt::gray));
	ui->comboBox_penColor->addItem(tr("Light Gray"), static_cast<int>(Qt::lightGray));
	ui->comboBox_penColor->addItem(tr("Red"), static_cast<int>(Qt::red));
	ui->comboBox_penColor->addItem(tr("Green"), static_cast<int>(Qt::green));
	ui->comboBox_penColor->addItem(tr("Blue"), static_cast<int>(Qt::blue));
	ui->comboBox_penColor->addItem(tr("Cyan"), static_cast<int>(Qt::cyan));
	ui->comboBox_penColor->addItem(tr("Magenta"), static_cast<int>(Qt::magenta));
	ui->comboBox_penColor->addItem(tr("Yellow"), static_cast<int>(Qt::yellow));
	ui->comboBox_penColor->addItem(tr("Dark Red"), static_cast<int>(Qt::darkRed));
	ui->comboBox_penColor->addItem(tr("Dark Green"), static_cast<int>(Qt::darkGreen));
	ui->comboBox_penColor->addItem(tr("Dark Blue"), static_cast<int>(Qt::darkBlue));
	ui->comboBox_penColor->addItem(tr("Dark Cyan"), static_cast<int>(Qt::darkCyan));
	ui->comboBox_penColor->addItem(tr("Dark Magenta"), static_cast<int>(Qt::darkMagenta));
	ui->comboBox_penColor->addItem(tr("Dark Yellow"), static_cast<int>(Qt::darkYellow));
	ui->comboBox_penColor->addItem(tr("Transparent"), static_cast<int>(Qt::transparent));

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
	connect(ui->comboBox_penColor, QOverload<int>::of(&QComboBox::activated), this, &QPenDialog::penChanged);
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
}

void QPenDialog::setCurrentPen(const QPen &apen) {
	// update widgets accordingly
	ui->spinBox_penWidth->setValue(apen.width());
	//ui->comboBox_penColor->setCurrentIndex(ui->comboBox_penStyle->findData(static_cast<int>(apen.color())));
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
	QColor color = QColor::fromRgba(ui->comboBox_penColor->itemData(ui->comboBox_penColor->currentIndex(), Qt::UserRole).toUInt());
	Qt::PenStyle style = Qt::PenStyle(ui->comboBox_penStyle->itemData(ui->comboBox_penStyle->currentIndex(), Qt::UserRole).toInt());
	Qt::PenCapStyle cap = Qt::PenCapStyle(ui->comboBox_penCap->itemData(ui->comboBox_penCap->currentIndex(), Qt::UserRole).toInt());
	Qt::PenJoinStyle join = Qt::PenJoinStyle(ui->comboBox_penJoin->itemData(ui->comboBox_penJoin->currentIndex(), Qt::UserRole).toInt());

	pen->setWidth(width);
	qDebug() << ui->comboBox_penColor->itemData(ui->comboBox_penColor->currentIndex(), Qt::UserRole);
	qDebug() << ui->comboBox_penColor->itemData(ui->comboBox_penColor->currentIndex(), Qt::UserRole).toInt();
	qDebug() << color;
	pen->setColor(color);
	pen->setStyle(style);
	pen->setCapStyle(cap);
	pen->setJoinStyle(join);

	emit currentPenChanged(*pen);
}


