//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_BINARYTREE_H
#define GUI_APP_BINARYTREE_H


#include <QtCore>
#include <QtMath>
#include "Node.hpp"


class BinaryTree {

public:
	explicit BinaryTree(QString expression);

	explicit BinaryTree();

	~BinaryTree();

	double calculateTree(double &xPlug);

	QVector<double> calculateTree(QVector<double> &xArray);

	Node *pRoot;

	void setPRoot(QString &expression);

private:
	void getNodeStats(Node *node);
};


#endif //GUI_APP_BINARYTREE_H