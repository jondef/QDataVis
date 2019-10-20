//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_NODE_H
#define GUI_APP_NODE_H

#include <QtCore>
#include <QtMath>
#include <complex.h>


class Node {
public:
	Node *pParent = nullptr;
	Node *pLeftChild = nullptr;
	Node *pRightChild = nullptr;

	QString strValue;
	double doubleValue;
	QString mathOperation = nullptr;

public:
	explicit Node(QString &input, Node *aParent = nullptr);

	bool createChildren(QString string);

private:
	static QList<int> getParenthesesArray(const QString &string);

	static QList<int> findAllOccurences(QString string, const QString &ofWhat);

	bool needsChildren();
};


#endif //GUI_APP_NODE_H