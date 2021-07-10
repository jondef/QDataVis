//
// Created by John on 12/06/2019.
//


#include <QtConcurrent/QtConcurrent>
#include "MainWindow.hpp"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::uiMainWindow) {
    ui->setupUi(this);

    setWindowIcon(QIcon(":/images/QDataVis"));

    plotWindow->setUpAxesPageConnections();
    plotWindow->setUpGeneralPageConnections();
    plotWindow->setUpTitlePageConnections();

    // * listWidgetItem doubled clicked
    connect(ui->listWidget_PointGraphList, &QListWidget::itemDoubleClicked, this, &MainWindow::graphDoubleClicked);
    connect(ui->QListWidget_functionList, &QListWidget::itemDoubleClicked, this, &MainWindow::graphDoubleClicked);

    // * points tab
    connect(ui->QPushButton_AddPointGraph, &QPushButton::clicked, this, &MainWindow::addPointsGraph);
    connect(ui->QPushButton_RemovePointGraph, &QPushButton::clicked, this, &MainWindow::removePointGraph);
    connect(ui->pushButton_regression, &QPushButton::clicked, this, &MainWindow::addRegression);

    // * function tab
    connect(ui->QTextEdit_functionInput, &QTextEditCustom::inputAccepted, ui->QPushButton_addFunction, &QPushButton::click);
    connect(ui->QPushButton_addFunction, &QPushButton::clicked, this, [this]() {
        addFunctionGraph(ui->QTextEdit_functionInput->toPlainText().simplified());
    });
    connect(ui->QPushButton_deleteFunction, &QPushButton::clicked, this, &MainWindow::removeFunctionGraph);
    connect(ui->spinBox_setGlobalPointDensity, qOverload<int>(&QSpinBox::valueChanged), ui->customPlot,
            &QCustomPlotCustom::globalPointDensityChanged);

    // * settings tab & general
    connect(ui->checkBox_settingsDarkMode, &QCheckBox::toggled, this, &MainWindow::updateColors);
    connect(ui->checkBox_stickAxisToZeroLines, &QCheckBox::toggled, ui->customPlot, &QCustomPlotCustom::stickAxisToZeroLine);
    connect(ui->checkBox_enableCursor, &QCheckBox::toggled, ui->customPlot, &QCustomPlotCustom::setCursor);
    connect(ui->pushButton_centerPlot, &QPushButton::clicked, ui->customPlot, [this](){
        ui->customPlot->setPlotRange(QCPRange(-10,10), QCPRange(-10,10));
    });
    connect(pointGraphDialog, &PointWindow::currentGraphChanged, this, [this]() {
        ui->customPlot->replotGraphsOnRangeChange();
    });

    // region menubar connections
    connect(ui->actionExport_data, &QAction::triggered, this, &MainWindow::exportData);
    connect(ui->actionImport_data, &QAction::triggered, this, &MainWindow::importData);
    connect(ui->actionQuit, &QAction::triggered, QApplication::instance(), &QApplication::quit);
    connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::savePlotImage);
    connect(ui->actionProperties, &QAction::triggered, this, [this]() {
        plotWindow->show();
        plotWindow->raise(); // bring it to front
        plotWindow->activateWindow(); // select it
        plotWindow->setWindowState(plotWindow->windowState() & ~Qt::WindowMinimized | Qt::WindowActive); // set to active
    });
    connect(ui->actionHelp, &QAction::triggered, this, []() {
        QDesktopServices::openUrl(QUrl("https://github.com/jondef/QDataVis", QUrl::TolerantMode));
    });
    connect(ui->actionAbout, &QAction::triggered, this, [this]() {
        QMessageBox::information(this, "About",
                                 "Version: " VERSION "\n"
                                 "Build date: " BUILDDATE "\n"
                                 "Git commit date: " GIT_COMMIT_DATE "\n"
                                 "Git commit hash: " GIT_COMMIT_HASH "\n"
                                 "Git branch: " GIT_BRANCH);
    });
    // endregion

    // emit the signal of the checkbox to update the plot colors
    emit(ui->checkBox_settingsDarkMode->toggled(false));
    readSettings();
}


MainWindow::~MainWindow() {
    delete ui;
    delete plotWindow;
    delete pointGraphDialog;
}


void MainWindow::readSettings() {
    QSettings settings;

    settings.beginGroup("settings");
    ui->checkBox_settingsDarkMode->setChecked(settings.value("darkModeEnabled").toBool());
    ui->checkBox_settingsOpenGL->setChecked(settings.value("useOpenGL").toBool());
    settings.endGroup();
}


void MainWindow::writeSettings() {
    QSettings settings;

    settings.beginGroup("settings");
    settings.setValue("darkModeEnabled", ui->checkBox_settingsDarkMode->isChecked());
    settings.setValue("useOpenGL", ui->checkBox_settingsOpenGL->isChecked());
    settings.endGroup();
}


void MainWindow::setSelectedDataSet(DataSet *dataSet) {
    if (dataSet == nullptr) {
        ui->QListWidget_functionList->setCurrentItem(nullptr);
        ui->listWidget_PointGraphList->setCurrentItem(nullptr);
        return;
    }
    // if dataset is a function
    if (dataSet->binaryTree != nullptr) {
        ui->tabWidget->setCurrentIndex(0);
        ui->QListWidget_functionList->setCurrentItem(dataSet->listWidgetItem);
    } else { // if dataset is a point dataset
        ui->tabWidget->setCurrentIndex(1);
        ui->listWidget_PointGraphList->setCurrentItem(dataSet->listWidgetItem);
    }
}

void MainWindow::graphDoubleClicked(QListWidgetItem *listWidgetItem) {
    pointGraphDialog->setGraph(listWidgetItem);
    pointGraphDialog->show();
}

/**
 * This function enables or disables dark mode
 * @param checked: checkbox checked?
 */
void MainWindow::updateColors(bool checked) {
    setAutoFillBackground(true);
    if (checked) { // enabled dark mode // window = background, windowText = foreground
        QColor foreground(255, 255, 255);
        QColor background(50, 50, 50);

        QPalette pal = QApplication::palette();
        pal.setColor(QPalette::Window, background); // now black
        pal.setColor(QPalette::WindowText, foreground); // now white

        pal.setColor(QPalette::Base, QColor(60, 60, 60));
        pal.setColor(QPalette::AlternateBase, QColor(75, 75, 75));

        pal.setColor(QPalette::Button, QColor(75, 75, 75));
        pal.setColor(QPalette::ButtonText, foreground);

        pal.setColor(QPalette::ToolTipBase, QColor(72, 98, 140));
        pal.setColor(QPalette::ToolTipText, foreground);

        pal.setColor(QPalette::Text, foreground);
        pal.setColor(QPalette::PlaceholderText, QColor(150, 150, 150));
        pal.setColor(QPalette::BrightText, QColor(150, 150, 150));

        QApplication::setPalette(pal);
    } else { // dark mode disabled
        QApplication::setPalette(QApplication::style()->standardPalette());
    }
    ui->customPlot->updateColors();
}


void MainWindow::exportData() {
    QString savePathFilename = QFileDialog::getSaveFileName(this, tr("Export data"), "", tr("*.dat"));
    if (savePathFilename.isEmpty()) { return; }

    QFile out_file(savePathFilename);
    if (!out_file.open(QFile::WriteOnly)) {
        QMessageBox::warning(nullptr, "Error", tr("\n Could not create file on disk"));
        return;
    }
    QDataStream out(&out_file);
    // Write a header with a "magic number" and a version
    out << (quint32)0xA0B0C0D0;
    out << (qint32)123;

    out.setVersion(QDataStream::Qt_6_1);

    // write the amount of saved data sets
    out << (quint32)ui->customPlot->mDataSets.size();

    // Write the data
    for (DataSet *dataSet : ui->customPlot->mDataSets) {
        out << *dataSet;
    }
    out_file.close();
}


void MainWindow::importData() {
    QString savePathFilename = QFileDialog::getOpenFileName(this, tr("Import data"), "", tr("*.dat"));
    if (savePathFilename.isEmpty()) { return; }

    QFile in_file(savePathFilename);
    if(!in_file.open(QFile::ReadOnly)) {
        QMessageBox::warning(nullptr, "Error", tr("\n Could not read file from disk"));
        return;
    }
    QDataStream in(&in_file);

    // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != 0xA0B0C0D0) {
        return;
    }

    // Read the version
    qint32 version;
    in >> version;
    if (version < 100) {
        return;
    }

    in.setVersion(QDataStream::Qt_6_1);

    // read the amount of stored data sets
    qint32 amount_of_dataSets;
    in >> amount_of_dataSets;

    // Read the data
    for (int i = 0; i < amount_of_dataSets; ++i) {
        DataSet dataSet;
        in >> dataSet;
        addFunctionGraph(dataSet.displayName);
    }

    in_file.close();
}


void MainWindow::savePlotImage() {
    QString savePathFilename = QFileDialog::getSaveFileName(this, tr("Save plot"), "", tr("*.jpg;;*.png;;*.bmp;;*.pdf"));
    if (savePathFilename.isEmpty()) { return; }

    bool savedOk = false;
    if (savePathFilename.toLower().endsWith(".png")) {
        savedOk = ui->customPlot->savePng(savePathFilename, 0, 0, 3.0, 100);
    } else if (savePathFilename.toLower().endsWith(".jpg")) {
        savedOk = ui->customPlot->saveJpg(savePathFilename, 0, 0, 3.0, -1); // fixme: save as jpg doesn't work
    } else if (savePathFilename.toLower().endsWith(".bmp")) {
        savedOk = ui->customPlot->saveBmp(savePathFilename, 0, 0, 3.0);
    } else if (savePathFilename.toLower().endsWith(".pdf")) {
        savedOk = ui->customPlot->savePdf(savePathFilename, 0, 0, QCP::epAllowCosmetic, QString(""), QString("Title"));
    }
    if (savedOk) {
        statusBarMsg(QString("Successfully saved plot to %1").arg(savePathFilename), 5000);
    } else {
        QMessageBox::warning(nullptr, "Error", tr("\nAn error occured while saving"));
    }
}

void MainWindow::addRegression() {
    QListWidgetItem *selectedListWidgetItem = ui->listWidget_PointGraphList->currentItem();
    if (!selectedListWidgetItem) {
        statusBarMsg("Select a dataset on which to do linear regression");
        return;
    }
    DataSet *selectedDataSet = selectedListWidgetItem->data(Qt::UserRole).value<DataSet *>();

    QFuture<QString> f = QtConcurrent::run(QThreadPool::globalInstance(), [this, selectedDataSet]() {
        return DataSet::getFunctionString(selectedDataSet->regression(ui->spinBox_regressionDegree->value() + 1));
    });
    QFutureWatcher<QString> *watcher = new QFutureWatcher<QString>;
    connect(watcher, &QFutureWatcher<QString>::finished, this, [this, watcher]() {
        addFunctionGraph(watcher->result());
        watcher->deleteLater();
    });
    watcher->setFuture(f);
}

void MainWindow::addFunctionGraph(QString func) {
    if (func.isEmpty()) return;
    QListWidgetItem *listWidgetItem = new QListWidgetItem();

    ui->customPlot->addFunctionGraph(func, listWidgetItem);
    ui->QListWidget_functionList->addItem(listWidgetItem);
}

void MainWindow::addPointsGraph() {
    QListWidgetItem *listWidgetItem = new QListWidgetItem();
    QString graphName = QString("Graph #%1").arg(ui->listWidget_PointGraphList->count() + 1);

    ui->customPlot->addPointsGraph(graphName, listWidgetItem);
    ui->listWidget_PointGraphList->addItem(listWidgetItem);
}

void MainWindow::removeFunctionGraph() {
    QListWidgetItem *selectedItem = ui->QListWidget_functionList->currentItem();
    if (selectedItem) {
        ui->customPlot->deleteGraph(selectedItem->data(Qt::UserRole).value<DataSet *>());
        delete selectedItem;
    }
}

void MainWindow::removePointGraph() {
    QListWidgetItem *selectedItem = ui->listWidget_PointGraphList->currentItem();
    if (selectedItem) {
        ui->customPlot->deleteGraph(selectedItem->data(Qt::UserRole).value<DataSet *>());
        delete selectedItem;
    }
}