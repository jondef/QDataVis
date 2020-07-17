//
// Created by John on 12/06/2019.
//

#include "BinaryTree.hpp"


/*
 * todo: implement heap binary tree
 * https://en.wikipedia.org/wiki/Binary_heap#/media/File:Binary_Heap_with_Array_Implementation.JPG
 *
 * 2n   -> left child
 * 2n+1 -> right child
 * n/2  -> parent
 */

BinaryTree::BinaryTree(QString expression) {
	expression.remove(" ");
	pRoot = new Node(expression);
}

BinaryTree::~BinaryTree() {
	delete pRoot;
}

/**
 * returns f(x)
 * @param xPlug: x value to insert in function
 * @return y
 */
double BinaryTree::calculateTree(double &xPlug) const {
	Node *currentNode = pRoot;
	Node *previousNode = nullptr;
	bool returning = false;

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


QVector<double> BinaryTree::calculateTree(QVector<double> &xArray) const {
	QVector<double> yArray(xArray.length());

	for (int i = 0; i < xArray.length(); ++i) {
		yArray[i] = calculateTree(xArray[i]);
	}
	return yArray;
}


[[maybe_unused]] void BinaryTree::getNodeStats(Node *node) {
	qDebug() << "pParent: " << node->pParent << "\tpLeft_child: " << node->pLeftChild << "\tpRight_child: " << node->pRightChild;
	qDebug() << "operation: [" << node->pLeftChild->strValue << " (" << node->pLeftChild->doubleValue << ")] " << node->mathOperation << " ["
			 << node->pRightChild->strValue << " (" << node->pRightChild->doubleValue << ")]\n";
}

void BinaryTree::printTree() const {
	QVector<QVector<QChar>> grid;

	Node *currentNode = pRoot;
	Node *previousNode = nullptr;
	bool returning = false;
	QPoint rootCoord = QPoint();
	QPoint mostLeftNodeCoord = QPoint();


	auto getChar = [&grid](int x, int y) -> QChar { return grid.at(y).at(x); };
	auto setChar = [&grid, getChar](Node *node, QPoint coord) { getChar(0, 0); };

	// go as far left as possible
	while (currentNode->pLeftChild) {
		currentNode = currentNode->pLeftChild;
		rootCoord = QPoint(rootCoord.x() + 2, 0);
		mostLeftNodeCoord = QPoint(0, mostLeftNodeCoord.y() + 2);
	}
	// go back to root
	currentNode = pRoot;





	// print the grid
	QString result;
	for (const QVector<QChar> &line : grid) {
		for (const QChar &character : line) {
			result.append(character);
		}
		result.append('\n');
	}
	qDebug() << result;
}

