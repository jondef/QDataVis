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

	~BinaryTree();

	static void explicitMaths(QString &expression);

	double calculateTree(double &xPlug) const;

	QVector<double> calculateTree(QVector<double> &xArray) const;

	Node *pRoot;

	void printTree() const;

private:
	[[maybe_unused]] static void getNodeStats(Node *node);
};


#endif //GUI_APP_BINARYTREE_H