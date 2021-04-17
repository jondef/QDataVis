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

    static void preprocessor(QString &expression);

    double calculateTree(const double &xPlug) const;

    QVector<double> calculateTree(const QVector<double> &xArray) const;

    void printTree() const;

private:
    [[maybe_unused]] static void getNodeStats(Node *node);

private:
    Node *pRoot;
};


#endif //GUI_APP_BINARYTREE_H