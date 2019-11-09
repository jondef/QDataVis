//
// Created by John on 12/06/2019.
//

#include "BinaryTree.h"


BinaryTree::BinaryTree(QString &expression) {
	pRoot = new Node(expression);
}

BinaryTree::BinaryTree() = default;


void BinaryTree::setPRoot(QString &expression) {
	pRoot = new Node(expression);
}

/**
 * returns f(x)
 * @param xPlug: x value to insert in function
 * @return y
 */
double BinaryTree::calculateTree(double &xPlug) {
	static Node *currentNode;
	currentNode = pRoot;
	static Node *previousNode = nullptr;

	static bool returning = false;

	while (true) {
		if (currentNode->pLeftChild && !returning) {
			currentNode = currentNode->pLeftChild;
		} else if (currentNode->pRightChild && !returning) {
			currentNode = currentNode->pRightChild;
		}

		// if node has no children
		if (!currentNode->pLeftChild && !currentNode->pRightChild) {
			returning = true;
		}

		if (returning) {
			if (currentNode->pParent) {
				previousNode = currentNode;
				currentNode = currentNode->pParent;
			}

			// only go right if we haven't been right
			if (currentNode->pRightChild && currentNode->pRightChild != previousNode) {
				returning = false;
				currentNode = currentNode->pRightChild;
				continue;
			}
			currentNode->doubleValue = currentNode->computeOperation(xPlug);
		}

		if (currentNode == pRoot && returning) {
			return currentNode->doubleValue;
		}
	}
}


QVector<double> BinaryTree::calculateTree(QVector<double> &xArray) {
	QVector<double> yArray(xArray.length());

	for (int i = 0; i < xArray.length(); ++i) {
		yArray[i] = calculateTree(xArray[i]);
	}
	return yArray;
}


void BinaryTree::getNodeStats(Node *node) {
	qDebug() << "pParent: " << node->pParent << "\tpLeft_child: " << node->pLeftChild << "\tpRight_child: " << node->pRightChild;
	qDebug() << "operation: [" << node->pLeftChild->strValue << " (" << node->pLeftChild->doubleValue << ")] " << node->mathOperation << " ["
	         << node->pRightChild->strValue << " (" << node->pRightChild->doubleValue << ")]\n";
}

