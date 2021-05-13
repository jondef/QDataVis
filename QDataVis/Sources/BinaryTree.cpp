//
// Created by John on 12/06/2019.
//

#include <plottables/plottable-graph.h>
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
    preprocessor(expression);
    pRoot = new Node(expression);
}

BinaryTree::~BinaryTree() {
    delete pRoot;
}

/**
 * This functions makes changes to a mathematical expression
 * to make it more explicit. For example:
 * -x-1 will become +-x+-1
 * 3e^((5x+3)(-6x4)) will become 3*e^((5*x+3)*(-6*x*4))
 */
void BinaryTree::preprocessor(QString &expression) {
    expression.remove(" ");
    expression.replace("-", "+-");
    expression.replace(")(", ")*(");

    QRegularExpression regex = QRegularExpression("((?<=\\W)[a-zA-Z][0-9(])|([0-9)][a-zA-Z])");
    while (expression.indexOf(regex) != -1) {
        int index = expression.indexOf(regex) + 1;
        expression.insert(index, '*');
    }

    // make the implicit multiply symbol explicit
    for (const QString &symbol : {"x", "pi", "e"}) {
        // replace with positive look-behind
        expression.replace(QRegularExpression(QString("(?<=\\d|\\))%1{1}").arg(symbol)), QString("*%1").arg(symbol));
        // replace with positive look ahead
        expression.replace(QRegularExpression(QString("%1{1}(?=\\d|\\()").arg(symbol)), QString("%1*").arg(symbol));
    }
    if (expression.isEmpty()) {
        expression = "0";
    }
}

/**
 * returns f(x)
 * @param xPlug: x value to insert in function
 * @return y
 */
double BinaryTree::calculateTree(const double &xPlug) const {
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


QVector<double> BinaryTree::calculateTree(const QVector<double> &xArray) const {
    QVector<double> yArray(xArray.length());

    for (int i = 0; i < xArray.length(); ++i) {
        yArray[i] = calculateTree(xArray[i]);
    }
    return yArray;
}


QSharedPointer<QCPGraphDataContainer> BinaryTree::calculateTree(double lowerLim, double upperLim, unsigned int length) const {
    double difference = upperLim - lowerLim;
    double increment = difference / (length - 1);

    QVector<QCPGraphData> tempData(length);
    QVector<QCPGraphData>::iterator it = tempData.begin();

    for (unsigned int i = 0; i < length; ++i) {
        it->key = lowerLim + increment * i;
        it->value = calculateTree(it->key);
        ++it;
    }
    QSharedPointer<QCPGraphDataContainer> data = QSharedPointer<QCPGraphDataContainer>(new QCPGraphDataContainer);
    data->set(tempData, true);
    return data;
}


[[maybe_unused]] void BinaryTree::getNodeStats(Node *node) {
    qDebug() << "pParent: " << node->pParent << "\tpLeft_child: " << node->pLeftChild << "\tpRight_child: " << node->pRightChild;
    qDebug() << "operation: [" << node->pLeftChild->strValue << " (" << node->pLeftChild->doubleValue << ")] " << node->mathOperation << " ["
             << node->pRightChild->strValue << " (" << node->pRightChild->doubleValue << ")]\n";
}

void BinaryTree::printTree() const {
    QVector<QPair<QPoint, Node *>> coords;
    // region Navigate the tree and at every node save its coordinates to the coords vector
    {
        Node *currentNode = pRoot;
        Node *previousNode = nullptr;
        bool returning = false;

        int offset = 5;
        QPoint currentPosition = QPoint(0, 0);
        while (true) {
            if (currentNode->pLeftChild && !returning) {
                currentPosition.setX(currentPosition.x() - offset);
                currentPosition.setY(currentPosition.y() - offset);
                currentNode = currentNode->pLeftChild;
            } else if (currentNode->pRightChild && !returning) {
                currentPosition.setX(currentPosition.x() + offset);
                currentPosition.setY(currentPosition.y() - offset);
                currentNode = currentNode->pRightChild;
            }
            // if node has no children
            if (!currentNode->pLeftChild && !currentNode->pRightChild) {
                coords.append(QPair<QPoint, Node *>(currentPosition, currentNode));
                returning = true;
            }
            if (returning) {
                if (currentNode->pParent) {
                    previousNode = currentNode;
                    currentNode = currentNode->pParent;
                    // if we're coming from the left child
                    if (previousNode == currentNode->pLeftChild) {
                        currentPosition.setX(currentPosition.x() + offset);
                        currentPosition.setY(currentPosition.y() + offset);
                    } else { // if we're coming from the right child
                        currentPosition.setX(currentPosition.x() - offset);
                        currentPosition.setY(currentPosition.y() + offset);
                    }

                }
                // only go right if we haven't been right
                if (currentNode->pRightChild && currentNode->pRightChild != previousNode) {
                    returning = false;
                    currentPosition.setX(currentPosition.x() + offset);
                    currentPosition.setY(currentPosition.y() - offset);
                    currentNode = currentNode->pRightChild;
                    continue;
                }
                coords.append(QPair<QPoint, Node *>(currentPosition, currentNode));
            }
            if (currentNode == pRoot && returning) {
                break; // done navigating the tree
            }
        }
    }
    // endregion
    // region get the nodes that are the farthest from the origin
    int leftMostNodeX = 0;
    int bottomMostNodeY = 0;
    int rightMostNodeX = 0;
    for (const QPair<QPoint, Node *> &pair : coords) {
        if (pair.first.x() < leftMostNodeX) {
            leftMostNodeX = pair.first.x();
        } else if (pair.first.x() > rightMostNodeX) {
            rightMostNodeX = pair.first.x();
        }
        if (pair.first.y() < bottomMostNodeY) {
            bottomMostNodeY = pair.first.y();
        }
    }
    // endregion

    int gridWidth = rightMostNodeX - leftMostNodeX;
    int gridHeight = 0 - bottomMostNodeY;
    QVector<QVector<QString>> grid(gridHeight + 1, QVector<QString>(gridWidth + 1, "   "));

    auto getChar = [&grid](QPoint point) -> QString * {
        qDebug() << "Accessing " << point;
        return &(grid[point.y()][point.x()]);
    };

    /**
     * Example input: (-2,-2)
     * 1. Move origin on xAxis (0, -2) (coordX - leftMostX, y)
     * 2. Flip y Axis (0, 2) (newCoordX, -y)
     */
    auto mapCoordToArray = [&leftMostNodeX](QPoint point) -> QPoint {
        return QPoint(point.x() - leftMostNodeX, -point.y());
    };

    auto getOperationChar = [](Operator anOperator) -> QString {
        const QPair<QString, Operator> *pair = std::find_if(operatorsPriority.begin(), operatorsPriority.end(),
                                                            [&](const QPair<QString, Operator> &pairIterating) {
                                                                return pairIterating.second == anOperator;
                                                            });
        if (pair) {
            return QString(" %1 ").arg(pair->first);
        } else {
            return std::find_if(specialOperatorsPriority.begin(), specialOperatorsPriority.end(),
                                [&](const QPair<QString, Operator> &pairIterating) { return pairIterating.second == anOperator; })->first;
        }
    };

    auto setChar = [&getChar, &getOperationChar, &mapCoordToArray](QPoint coord, Node *node) {
        if (node->mathOperation != -1) { // if node is a math operation
            *(getChar(mapCoordToArray(coord))) = getOperationChar(node->mathOperation);
        } else if (node->isVariable) {
            *(getChar(mapCoordToArray(coord))) = " x ";
        } else { // if node is a constant
            *(getChar(mapCoordToArray(coord))) = QString::number(node->doubleValue, 'g', 3);
        }

    };
    // fill the grid with the values
    for (const QPair<QPoint, Node *> &pair : coords) {
        // set the node value
        setChar(pair.first, pair.second);
        // set the lines
        Node *currentNode = pair.second;
        QPoint currentPoint = pair.first;
        if (currentNode->pLeftChild) {
            QPoint leftChildCoord = std::find_if(coords.begin(), coords.end(), [&](const QPair<QPoint, Node *> &pairIterating) {
                return (pairIterating.second->pParent == currentNode);
            })->first;
            // since it's the left child, leftChildCoord < currentPoint
            for (int i = 1; i < currentPoint.x() - leftChildCoord.x(); ++i) {
                *(getChar(mapCoordToArray(QPoint(leftChildCoord.x() + i, leftChildCoord.y() + i)))) = " / ";
            }
        }
        if (currentNode->pRightChild) {
            QPoint rightChildCoord = std::find_if(coords.begin(), coords.end(), [&](const QPair<QPoint, Node *> &pairIterating) {
                return currentNode->pRightChild == pairIterating.second;
            })->first;
            // since it's the right child, rightChildCoord > currentPoint
            for (int i = 1; i < rightChildCoord.x() - currentPoint.x(); ++i) {
                *(getChar(mapCoordToArray(QPoint(currentPoint.x() + i, currentPoint.y() - i)))) = " \\ ";
            }
        }
    }
    // print the grid
    QString result;
    for (const QVector<QString> &line : grid) {
        for (const QString &character : line) {
            result.append(character);
        }
        result.append("\n");
    }
    qDebug() << result.toUtf8().constData();
}

