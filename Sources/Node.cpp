//
// Created by John on 12/06/2019.
//

#include "Node.h"


Node::Node(QString &aInput, Node *aParent) {

	if (aParent != nullptr) {
		pParent = aParent;
	}

	splitNodeValue(aInput);
	convertToDouble();
}


void Node::splitNodeValue(QString &string) {
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
				strValueLeft = leftSide;
				strValueRight = rightSide;
				mathOperation = operation;
				qDebug() << strValueLeft << mathOperation << strValueRight;
				return;
			}
		}
	}
	throw std::logic_error("Error @ split nodeValue function: Unable to find split");
}

void Node::convertToDouble() {
	doubleValueLeft = strValueLeft.toDouble(&valueLeftOk);
	doubleValueRight = strValueRight.toDouble(&valueRightOk);

	// creation of children
	// if value left is not ok and value is not x
	if (!valueLeftOk && strValueLeft != "x") {
		if (strValueLeft == "e") {
			valueLeftOk = true;
			doubleValueLeft = M_E;
		} else if (strValueLeft == "pi") {
			valueLeftOk = true;
			doubleValueLeft = M_PI;
		} else {
			hasLeftChild = true;
			pLeftChild = new Node(strValueLeft, this);
		}
	}

	if (!valueRightOk && strValueRight != "x") {
		if (strValueRight == "e") {
			valueRightOk = true;
			doubleValueRight = 2.718281828459045;
		} else if (strValueRight == "pi") {
			valueRightOk = true;
			doubleValueRight = 3.141592653589793;
		} else {
			hasRightChild = true;
			pRightChild = new Node(strValueRight, this);
		}
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


void Node::getNodeStats(double xPlug) {
	bool debug = false;
	if (debug) {
		qDebug() << "pParent: " << pParent << "\thasLeftChild: " << hasLeftChild << "\tpLeft_child: " << pLeftChild
				 << "\thasRightChild: " << hasRightChild << "\tpRight_child: " << pRightChild;
		qDebug() << "operation: [" << strValueLeft << " (" << doubleValueLeft << ")] " << mathOperation << " ["
				 << strValueRight << " (" << doubleValueRight << ")]\n";
	}
}

double Node::computeOperation(double &xPlug) {

	// replace x with the number
	if (!valueLeftOk) {
		doubleValueLeft = xPlug;
	}
	if (!valueRightOk) {
		doubleValueRight = xPlug;
	}

	if (mathOperation == "+") {
		return doubleValueLeft + doubleValueRight;
	} else if (mathOperation == "-") {
		return doubleValueLeft - doubleValueRight;
	} else if (mathOperation == "-") {
		return doubleValueLeft - doubleValueRight;
	} else if (mathOperation == "*") {
		return doubleValueLeft * doubleValueRight;
	} else if (mathOperation == "/") {
		// todo: add check for division by zero?
		return doubleValueLeft / doubleValueRight;
	} else if (mathOperation == "^") {
		//qDebug() << "-------------";
		//double _Complex a = cpow(-10, 1.0/2.0);
		//qDebug() << creal(a) << "+" << cimag(a) << "i =" << cabs(a) << conj(a) << carg(a);
		//double _Complex b = cpow(-10, 1.0/3.0);
		//qDebug() << creal(b) << "+" << cimag(b) << "i =" << cabs(b) << conj(b) << carg(b);
		//double _Complex c = cpow(-10, 1.0/4.0);
		//qDebug() << creal(c) << "+" << cimag(c) << "i =" << cabs(c) << conj(c) << carg(c);
		//double _Complex d = cpow(-10, 1.0/5.0);
		//qDebug() << creal(d) << "+" << cimag(d) << "i =" << cabs(d) << conj(d) << carg(d);

		// ! don't use std::pow because it doesn't support negative
		// ! roots, however cubic_root(-6.5) should still work
		auto lam = [](double number) {
			auto number_timesTen = number * 10;
			auto numerator = number_timesTen - number;
			auto denominator = 9;
			qDebug() << numerator << "/" << denominator;
		};

//		lam(0.1234);

		//if (doubleValueLeft < 0) {
		//	return -qPow(-doubleValueLeft, doubleValueRight);
		//}
		return qPow(doubleValueLeft, doubleValueRight);
//		return cabs(cpow(doubleValueLeft, doubleValueRight));
	} else if (mathOperation == "sin") {
		return sin(doubleValueRight);
	} else if (mathOperation == "asin") {
		return asin(doubleValueRight);
	} else if (mathOperation == "cos") {
		return cos(doubleValueRight);
	} else if (mathOperation == "acos") {
		return acos(doubleValueRight);
	} else if (mathOperation == "tan") {
		return tan(doubleValueRight);
	} else if (mathOperation == "atan") {
		return atan(doubleValueRight);
	} else if (mathOperation == "log") {
		return log10(doubleValueRight);
	} else if (mathOperation == "ln") {
		return qLn(doubleValueRight);
	}
	return 0;
}


QList<int> Node::findAllOccurences(QString &string, const QString &ofWhat) {
	QList<int> operatorIndex;

	QString temp = string;
	int cutAwayParts = 0;

	while (temp.indexOf(ofWhat) != -1) {
		operatorIndex.append(temp.indexOf(ofWhat) + cutAwayParts);
		cutAwayParts += temp.left(temp.indexOf(ofWhat) + 1).length();

		temp = temp.mid(temp.indexOf(ofWhat) + 1);
	}

	return operatorIndex;
}