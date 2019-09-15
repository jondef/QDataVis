//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_NODE_H
#define GUI_APP_NODE_H

#include <QtCore>
#include <QtMath>

class Node {
public:
    explicit Node(QString input, Node *aParent = nullptr);

    void splitNodeValue(QString string);

	void getNodeStats(double xPlug);

    double computeOperation(double xPlug);

    Node *pParent = nullptr;

    Node *pLeftChild = nullptr;
    Node *pRightChild = nullptr;

    bool hasLeftChild = false;
    bool hasRightChild = false;

    double doubleValueLeft;
    double doubleValueRight;

    bool valueLeftOk;
    bool valueRightOk;

private:
    QString strValueLeft;
    QString strValueRight;

    QString mathOperation;

    void convertToDouble();

    static QList<int> getParenthesesArray(const QString &string);

    static QList<int> findAllOccurences(QString string, const QString &ofWhat);
};


#endif //GUI_APP_NODE_H