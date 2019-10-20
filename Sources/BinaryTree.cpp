//
// Created by John on 12/06/2019.
//

#include <QtWidgets/QProgressBar>
#include "BinaryTree.h"


BinaryTree::BinaryTree(QString &expression) {
	pRoot = new Node(expression);
}


double BinaryTree::calculateTree(double &xPlug) {
	// returns f(x)
	Node *currentNode = pRoot;

	bool returning = false;

	// append 1 if moved to left or 2 if moved to right
	// 0 is only a placeholder to avoid empty list
	QList<int> movement = {0};

	while (true) {
		// explore the tree
		if (currentNode->pLeftChild != nullptr && !returning) {
			movement.append(1);
			currentNode = currentNode->pLeftChild;
		} else if (currentNode->pRightChild != nullptr && !returning) {
			movement.append(2);
			currentNode = currentNode->pRightChild;
		}

		// if node has no children
		if (currentNode->pLeftChild == nullptr && currentNode->pRightChild == nullptr) {
			returning = true;
		}

		if (returning) {
			if (currentNode->pParent != nullptr) {
				currentNode = currentNode->pParent;
			}
			//50+(x+1)*(x+1)+10
			// try to go right
			if (movement.last() == 1) {
				//currentNode->doubleValue = computeOperation(currentNode->pLeftChild, xPlug);
				movement.removeLast();

				if (currentNode->pRightChild != nullptr) {
					returning = false;
					movement.append(2);
					currentNode = currentNode->pRightChild;
				}

			} else if (movement.last() == 2 && currentNode->pParent != nullptr) {
				movement.removeLast();
				currentNode->doubleValue = computeOperation(currentNode, xPlug);
			}
		}

		if (currentNode == pRoot && returning) {
			break;
		}

	}
	// ? might be useless
	// compute the values of root's children
	//if (currentNode->hasLeftChild) {
	//    currentNode->doubleValueLeft = currentNode->pLeftChild->computeOperation(xPlug);
	//    currentNode->valueLeftOk = true;
	//}
	currentNode->doubleValue = computeOperation(currentNode, xPlug);

	return currentNode->doubleValue;
}


QVector<double> BinaryTree::calculateTree(QVector<double> &xArray, QProgressBar *progressBar) {
	QVector<double> yArray(xArray.length());

	for (int i = 0; i < xArray.length(); ++i) {
		yArray[i] = calculateTree(xArray[i]);
		progressBar->setValue(i / float(xArray.length()) * 100);
	}
	progressBar->setValue(0);
	return yArray;
}


double BinaryTree::computeOperation(Node *node, double xPlug) {
	double doubleValueLeft = 0;
	double doubleValueRight = 0;

	// replace x with the number
	if (node->pLeftChild != nullptr) {
		if (node->pLeftChild->strValue == "x") {
			doubleValueLeft = xPlug;
		} else {
			doubleValueLeft = node->pLeftChild->doubleValue;
		}
	}

	if (node->pRightChild != nullptr) {
		if (node->pRightChild->strValue == "x") {
			doubleValueRight = xPlug;
		} else {
			doubleValueRight = node->pRightChild->doubleValue;
		}
	}

	if (node->mathOperation == "+") {
		return doubleValueLeft + doubleValueRight;
	} else if (node->mathOperation == "-") {
		return doubleValueLeft - doubleValueRight;
	} else if (node->mathOperation == "-") {
		return doubleValueLeft - doubleValueRight;
	} else if (node->mathOperation == "*") {
		return doubleValueLeft * doubleValueRight;
	} else if (node->mathOperation == "/") {
		// todo: add check for division by zero?
		return doubleValueLeft / doubleValueRight;
	} else if (node->mathOperation == "^") {
		// ! all the even denominator roots have imaginary negative
		// ! while odd denominator roots have real negatives
		// check if exponent is a root
//		if (strValueRight.contains("/")) {
//			int numerator = 1;
//			double denominator = numerator / doubleValueRight;
//
//			double originalNumerator = strValueRight.split("/").at(0).toDouble();
//			numerator = numerator * originalNumerator;
//			denominator = denominator * originalNumerator;
//
//			// check if denominator is even
//			if (int(denominator) % 2 == 0) {
//				// if it's even, the negative values are imaginary
//				return qPow(doubleValueLeft, doubleValueRight);
//			} else { // odd denominator
//				// if it's odd, the negative values are real
//				if (doubleValueLeft < 0) {
//					if (numerator % 2 == 0) {
//						return qPow(doubleValueLeft, doubleValueRight);
//					}
//					return -qPow(-doubleValueLeft, doubleValueRight);
//				} else {
//					return qPow(doubleValueLeft, doubleValueRight);
//				}
//			}
//		}
		return qPow(doubleValueLeft, doubleValueRight);
	} else if (node->mathOperation == "sin") {
		return sin(doubleValueRight);
	} else if (node->mathOperation == "asin") {
		return asin(doubleValueRight);
	} else if (node->mathOperation == "cos") {
		return cos(doubleValueRight);
	} else if (node->mathOperation == "acos") {
		return acos(doubleValueRight);
	} else if (node->mathOperation == "tan") {
		return tan(doubleValueRight);
	} else if (node->mathOperation == "atan") {
		return atan(doubleValueRight);
	} else if (node->mathOperation == "log") {
		return log10(doubleValueRight);
	} else if (node->mathOperation == "ln") {
		return qLn(doubleValueRight);
	}
	return 0;
}


void BinaryTree::getNodeStats(Node *node) {
	qDebug() << "pParent: " << node->pParent << "\tpLeft_child: " << node->pLeftChild << "\tpRight_child: " << node->pRightChild;
	qDebug() << "operation: [" << node->pLeftChild->strValue << " (" << node->pLeftChild->doubleValue << ")] " << node->mathOperation << " ["
			 << node->pRightChild->strValue << " (" << node->pRightChild->doubleValue << ")]\n";
}
