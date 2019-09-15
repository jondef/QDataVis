//
// Created by John on 12/06/2019.
//

#include <QtWidgets/QProgressBar>
#include "BinaryTree.h"


BinaryTree::BinaryTree(QString expression) {
    pRoot = new Node(expression);
}


double BinaryTree::calculateTree(double xPlug) {
    // returns f(x)

    Node *currentNode = pRoot;

    bool returning = false;

    // append 1 if moved to left or 2 if moved to right
    // 0 is only a placeholder to avoid empty list
    QList<int> movement = {0};

    while (true) {
        // explore the tree
        if (currentNode->hasLeftChild && !returning) {
            movement.append(1);
            currentNode = currentNode->pLeftChild;
        } else if (currentNode->hasRightChild && !returning) {
            movement.append(2);
            currentNode = currentNode->pRightChild;
        }

        // if node has no children
        if (!currentNode->hasLeftChild && !currentNode->hasRightChild) {
            returning = true;
        }

        if (returning) {
            if (currentNode->pParent != nullptr) {
                currentNode = currentNode->pParent;
            }
            //50+(x+1)*(x+1)+10
            // try to go right
            if (movement.last() == 1) {
                currentNode->doubleValueLeft = currentNode->pLeftChild->computeOperation(xPlug);
                currentNode->valueLeftOk = true;
                movement.removeLast();

                if (currentNode->hasRightChild) {
                    returning = false;
                    movement.append(2);
                    currentNode = currentNode->pRightChild;
                }

            } else if (movement.last() == 2 && currentNode->pParent != nullptr) {
                movement.removeLast();
                currentNode->doubleValueRight = currentNode->pRightChild->computeOperation(xPlug);
                currentNode->valueRightOk = true;
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
    if (currentNode->hasRightChild) {
        currentNode->doubleValueRight = currentNode->pRightChild->computeOperation(xPlug);
        currentNode->valueRightOk = true;
    }
	currentNode->getNodeStats(currentNode->computeOperation(xPlug));
    return currentNode->computeOperation(xPlug);

}


QVector<double> BinaryTree::calculateTree(QVector<double> xArray, QProgressBar *progressBar) {
    QVector<double> yArray(xArray.length());

    for (int i = 0; i < xArray.length(); ++i) {
        yArray[i] = calculateTree(xArray[i]);
		progressBar->setValue(i / float(xArray.length()) * 100);
    }
	progressBar->setValue(0);
    return yArray;
}