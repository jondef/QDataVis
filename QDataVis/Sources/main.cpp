//
// Created by John on 12/06/2019.
//

#include <QApplication>
#include <QtCore>

#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	// these are used for qSettings
	QCoreApplication::setOrganizationName("QDataVis");
	QCoreApplication::setOrganizationDomain("qdatavis.com");
	QCoreApplication::setApplicationName("QDataVis");

	uint16_t result = 0;
	do {
		MainWindow window;
		window.show();
		result = QCoreApplication::exec();
	} while (result == 1337);

	return result;
}

