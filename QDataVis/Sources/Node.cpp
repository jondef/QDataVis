//
// Created by John on 12/06/2019.
//

#include "Node.h"

const QList<QString> operators = {"+", "-", "*", "/", "%", "^", "!"};
// ! all "arc" trigo functions need to be before the normal ones
// csc = 1/sin(x) | sec = 1/cos(x) | cot = 1/tan(x)
const QList<QString> specialOperators = {"arcsinh", "arccosh", "arctanh", "arcsin", "arccos", "arctan",
                                         "sinh", "cosh", "tanh", "sin", "cos", "tan",
                                         "log10", "ln", "log"};
// todo: add more functions, operators: http://www.partow.net/programming/exprtk/
// todo: add abs()


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
	// job: define attributes mathOperation and doubleValue

	// remove parentheses only if there are one of them at beginning and end without any other
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
		// aka if the whole expression is in useless parentheses
		if (!parenthesesArray.contains(0)) {
			if (string.at(0) == "(" && string.at(string.length() - 1) == ")") {
				QString x = string.mid(1);
				string = x.left(x.length() - 1);
			}
		}
	}

	QList<int> parenthesesArray = getParenthesesArray(string);

	// * normal operators
	for (const QString &Operator : operators) {
		QList<int> allOperatorOccurences = findAllOccurences(string, Operator);

		for (const int &operatorOccurence : allOperatorOccurences) {
			// check if operator is in a parentheses or not.
			if (parenthesesArray.at(operatorOccurence) == 0) {
				QString leftSide = string.left(operatorOccurence);
				QString rightSide = string.mid(operatorOccurence + 1);

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
				mathOperation = Operator;
				qDebug() << leftSide << mathOperation << rightSide;
				pLeftChild = new Node(leftSide, this);
				if (!rightSide.isEmpty()) {
					pRightChild = new Node(rightSide, this);
				}
				return true; // success
			}
		}
	}

	// * special operators | must be after normal operators!
	for (const auto &operation : specialOperators) {
		QList<int> allOperatorOccurences = findAllOccurences(string, operation);

		for (const int &operatorOccurence : allOperatorOccurences) {
			if (parenthesesArray.at(operatorOccurence) == 0) {
				QString parenthesesContent = string.left(string.length() - 1).mid(operatorOccurence + operation.length() + 1);

				if (operation == "log") {
					QString logBaseString = parenthesesContent.split(",").at(1);
					if (logBaseString == "e") {
						logBase = M_E;
					} else if (logBaseString == "pi") {
						logBase = M_PI;
					} else {
						logBase = logBaseString.toDouble();
					}
					parenthesesContent = parenthesesContent.split(",").at(0);

				}

				mathOperation = operation;
				qDebug() << mathOperation << parenthesesContent;
				pRightChild = new Node(parenthesesContent, this);
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
		// needs children
		return true;
	}
}


QList<int> Node::getParenthesesArray(const QString &string) {
	QList<int> list;

	int insideHowManyParentheses = 0;
	for (auto &i : string) {
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