//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_BINARYTREE_H
#define GUI_APP_BINARYTREE_H


#include "Node.h"
#include <QtCore>

class BinaryTree {
public:
    explicit BinaryTree(QString input);

    double calculateTree(double xPlug);

	QVector<double> calculateTree(QVector<double> xArray, QProgressBar *progessBar);

	Node *pRoot;
};


#endif //GUI_APP_BINARYTREE_H