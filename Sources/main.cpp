//
// Created by John on 12/06/2019.
//

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

	return QApplication::exec();
}