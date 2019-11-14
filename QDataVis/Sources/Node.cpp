//
// Created by John on 12/06/2019.
//

#include "Node.h"


Node::Node(QString &aInput, Node *aParent) {
	pParent = aParent;
	strValue = aInput;

	if (needsChildren()) {
		hasChildren = true;
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
	for (const QPair<QString, Operator> &Operator : operatorsPriority) {
		QList<int> operatorIndexes = findAllOccurences(string, Operator.first);

		for (const int &operatorIndex : operatorIndexes) {
			// check if operator is in a parentheses or not.
			if (parenthesesArray.at(operatorIndex) == 0) {
				QString leftSide = string.left(operatorIndex);
				QString rightSide = string.mid(operatorIndex + 1);

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
				mathOperation = Operator.second;
				qDebug() << leftSide << Operator.first << rightSide;
				pLeftChild = new Node(leftSide, this);
				if (!rightSide.isEmpty()) {
					pRightChild = new Node(rightSide, this);
				}
				return true; // success
			}
		}
	}

	// * special operators | must be after normal operators!
	for (const QPair<QString, Operator> &Operator : specialOperatorsPriority) {
		QList<int> operatorIndexes = findAllOccurences(string, Operator.first);

		for (const int &operatorIndex : operatorIndexes) {
			if (parenthesesArray.at(operatorIndex) == 0) {
				QString parenthesesContent = string.left(string.length() - 1).mid(operatorIndex + Operator.first.length() + 1);

				if (Operator.first == "log") {
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

				mathOperation = Operator.second;
				qDebug() << Operator.first << parenthesesContent;
				pRightChild = new Node(parenthesesContent, this);
				return true; // success
			}
		}
	}
	return false; // failed
}

bool Node::needsChildren() {
	// returns false if node doesn't need children
	bool valueOk;
	doubleValue = strValue.toDouble(&valueOk);

	// doesn't need children
	if (valueOk) {
		return false;
	} else {
		if (strValue == "x") {
			isVariable = true;
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

double Node::computeOperation(double xPlug) {
	double doubleValueLeft = 0;
	double doubleValueRight = 0;

	// replace x with the number
	if (pLeftChild) {
		if (pLeftChild->isVariable) {
			doubleValueLeft = xPlug;
		} else {
			doubleValueLeft = pLeftChild->doubleValue;
		}
	}

	if (pRightChild) {
		if (pRightChild->isVariable) {
			doubleValueRight = xPlug;
		} else {
			doubleValueRight = pRightChild->doubleValue;
		}
	}

	switch (mathOperation) {
		case Addition :
			return doubleValueLeft + doubleValueRight;
		case Subtraction :
			return doubleValueLeft - doubleValueRight;
		case Multiplication :
			return doubleValueLeft * doubleValueRight;
		case Division :
			return doubleValueLeft / doubleValueRight;
		case Modulo :
			if (int(doubleValueRight) != 0) {
				return int(doubleValueLeft) % int(doubleValueRight);
			}
			return _nan();
		case Exponent:
			// only change if the base is negative
			// ref: https://stackoverflow.com/a/8493081/10450514
			if (doubleValueLeft < 0) {
				// check whether the exponent is a fraction
				if (pRightChild->mathOperation == Division) {
					// It is not required to check whether doubleValue has been assigned
					// a value, because is will be already computed by the time the
					// program is here.
					double numerator = pRightChild->pLeftChild->doubleValue;
					double denominator = pRightChild->pRightChild->doubleValue;

					// ! all the even denominator roots have imaginary negative
					// ! while odd denominator roots have real negatives
					// check if denominator is an int + check if denominator is odd
					if (round(denominator) == denominator && int(denominator) % 2 != 0) {
						// if the denominator is odd, the negative values are real
						// if numerator is an int
						if (round(numerator) == numerator) {
							if (int(numerator) % 2 == 0) {
								// if numerator is even simply flip the
								// function above the y axis
								return qPow(-doubleValueLeft, doubleValueRight);
							} else {
								// if the numerator is odd, we need to calculate the points
								// by making them positive, because a negative base is not
								// possible.
								return -qPow(-doubleValueLeft, doubleValueRight);
							}
						}
					}
				}
			}
			return qPow(doubleValueLeft, doubleValueRight);
		case Factorial:
			// + 1 to match x!
			return tgamma(doubleValueLeft + 1);
		case Arcsinh:
			return asinh(doubleValueRight);
		case Arccosh:
			return acosh(doubleValueRight);
		case Arctanh:
			return atanh(doubleValueRight);
		case Arcsin:
			return asin(doubleValueRight);
		case Arccos:
			return acos(doubleValueRight);
		case Arctan:
			return atan(doubleValueRight);
		case Sinh:
			return sinh(doubleValueRight);
		case Cosh:
			return cosh(doubleValueRight);
		case Tanh:
			return tanh(doubleValueRight);
		case Sin:
			return sin(doubleValueRight);
		case Cos:
			return cos(doubleValueRight);
		case Tan:
			return tan(doubleValueRight);
		case Log10:
			return log10(doubleValueRight);
		case Log:
			return log10(doubleValueRight) / log10(logBase);
		case Ln:
			return qLn(doubleValueRight);
		case Abs:
			return abs(doubleValueRight);
		case Sqrt:
			return sqrt(doubleValueRight);
		case Cbrt:
			return cbrt(doubleValueRight);
		default:
			// node has no children, but has a value
			// functions that are just a number like 2
			// fixme: what if you just enter x?
			return doubleValue;
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