//
// Created by John on 12/06/2019.
//

#include "Node.h"


Node::Node(QString &aInput, Node *aParent) {
	pParent = aParent;
	strValue = aInput;

	// check if the node needs children
	if (needsChildren()) {
		createChildren(aInput);
	}
}


bool Node::createChildren(QString string) {
	// returns true if split successful, otherwise false
	// todo: add support for log, sin, etc...
	// job: define attributes mathOperation, strValueLeft and strValueRight

	// remove parentheses only if there are one of them at beginning and end
	// handle cases such as (x*2+x*4)
	if (findAllOccurences(string, "(").length() == 1 && string.at(0) == "(") {
		if (findAllOccurences(string, ")").length() == 1 && string.at(string.length() - 1) == ")") {
			string.remove(")");
			string.remove("(");
		}
	}

	{
		QList<int> parenthesesArray = getParenthesesArray(string);
		// handle cases such as ((-x^2-4*x+46)^(1/2)-4)
		// developed expressions / not entirely factored
		if (parenthesesArray.contains(2)) {
			if (string.at(0) == "(" && string.at(string.length() - 1) == ")") {
				QString x = string.mid(1);
				string = x.left(x.length() - 1);
			}
		}
	}

	QList<int> parenthesesArray = getParenthesesArray(string);

	QList<QString> operations = {"+", "-", "*", "/", "^"};

	for (const QString &operation : operations) {
		QList<int> indexOfOperators = findAllOccurences(string, operation);


		//qDebug() << indexOfOperators << parenthesesArray;

		for (int i : indexOfOperators) {
			// check if found operator is in a parentheses or not
			if (parenthesesArray.at(i) == 0) {
				QString leftSide = string.left(i);
				QString rightSide = string.mid(i + 1);

				// if you have -2*x you gotta add a zero on the left side
				if (leftSide.isEmpty()) {
					if (!rightSide.contains("+") && !rightSide.contains("-")) {
						leftSide = "0";
					} else if (rightSide.at(0) == "(") {
						leftSide = "0";
					} else {
						continue;
					}
				}
				mathOperation = operation;
				qDebug() << leftSide << mathOperation << rightSide;
				pLeftChild = new Node(leftSide, this);
				pRightChild = new Node(rightSide, this);
				return true; // success
			}
		}
	}
	return false; // failed
}

bool Node::needsChildren() {
	// returns false if node does'nt need children
	bool valueOk;
	doubleValue = strValue.toDouble(&valueOk);

	// does'nt need children
	if (valueOk) {
		return false;
	} else {
		if (strValue == "x") {
			return false;
		} else if (strValue == "e") {
			doubleValue = M_E;
			return false;
		} else if (strValue == "pi") {
			doubleValue = M_PI;
			return false;
		}
		// needs children for real
		return true;
	}
}


QList<int> Node::getParenthesesArray(const QString &string) {
	QList<int> list;

	int insideHowManyParentheses = 0;
	for (auto &&i : string) {
		if (i == '(') {
			insideHowManyParentheses++;
			list.append(insideHowManyParentheses);
		} else if (i == ')') {
			list.append(insideHowManyParentheses);
			insideHowManyParentheses--;
		} else {
			list.append(insideHowManyParentheses);
		}
	}

	return list;
}


QList<int> Node::findAllOccurences(QString string, const QString &ofWhat) {
	QList<int> operatorIndex;

	int cutAwayParts = 0;

	while (string.indexOf(ofWhat) != -1) {
		operatorIndex.append(string.indexOf(ofWhat) + cutAwayParts);
		cutAwayParts += string.left(string.indexOf(ofWhat) + 1).length();

		string = string.mid(string.indexOf(ofWhat) + 1);
	}
	return operatorIndex;
}