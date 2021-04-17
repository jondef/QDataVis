//
// Created by John on 12/06/2019.
//

#include "Node.hpp"


Node::Node(QString &aInput, Node *aParent) {
    sigmaExpand(aInput);
    pParent = aParent;
    strValue = aInput;

    if (needsChildren()) {
        hasChildren = true;
        createChildren(aInput);
    }
}

Node::~Node() {
    delete pLeftChild;
    delete pRightChild;
}

/**
 * job: define attributes mathOperation and doubleValue
 * @return true if split successful, otherwise false
 */
bool Node::createChildren(QString string) {
    QList<int> parenthesesArray = getParenthesesArray(string);
    // handle cases such as ((-x^2-4*x+46)^(1/2)-4)
    // aka if the whole expression is in useless parentheses
    if (!parenthesesArray.contains(0)) {
        if (string.at(0) == '(' && string.at(string.length() - 1) == ')') {
            string.chop(1); // remove 1 char from end of string
            string.remove(0, 1); // remove 1st char from string
        }
    }
    parenthesesArray = getParenthesesArray(string);

    // * normal operators
    for (const QPair<QString, Operator> &Operator : operatorsPriority) {
        QList<int> operatorIndexes = findAllOccurrences(string, Operator.first);

        for (const int &operatorIndex : operatorIndexes) {
            // if operator is not in a parentheses
            if (parenthesesArray.at(operatorIndex) == 0) {
                QString leftSide = string.left(operatorIndex);
                QString rightSide = string.mid(operatorIndex + 1);
                // if you have -2*x you gotta add a zero on the left side
                if (leftSide.isEmpty()) {
                    // if you have something like -2*x^4
                    if (!rightSide.contains('+') && !rightSide.contains('-')) {
                        leftSide = "0";
                    } else if (rightSide.at(0) == '(') { // if you have something like -(x+2)
                        leftSide = "0";
                    } else if (true) { // fixme: dirty fix
                        leftSide = "0";
                    } else {
                        continue; // we must continue because left side is empty
                    }
                }
                mathOperation = Operator.second;
//				qDebug() << leftSide << Operator.first << rightSide;
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
        QList<int> operatorIndexes = findAllOccurrences(string, Operator.first);

        for (const int &operatorIndex : operatorIndexes) {
            if (parenthesesArray.at(operatorIndex) == 0) {
                QString parenthesesContent = string.left(string.length() - 1).mid(operatorIndex + Operator.first.length() + 1);

                if (Operator.first == "log") {
                    QString logBaseString = parenthesesContent.split(',').at(1);
                    if (logBaseString == "e") {
                        logBase = M_E;
                    } else if (logBaseString == "pi") {
                        logBase = M_PI;
                    } else {
                        logBase = logBaseString.toDouble();
                    }
                    parenthesesContent = parenthesesContent.split(',').at(0);
                }

                mathOperation = Operator.second;
//                qDebug() << Operator.first << parenthesesContent;
                pRightChild = new Node(parenthesesContent, this);
                return true; // success
            }
        }
    }
    return false; // failed
}

/**
 * Unfold/expand sigma sign. Example:
 * sigma(n*cos(n*10*2*pi*x),n,1,5)
 * to
 * cos(20*pi*x)+2*cos(40*pi*x)+3*cos(60*pi*x)+4*cos(80*pi*x)+5*cos(100*pi*x)
 * @param aInput: function to expand
 */
void Node::sigmaExpand(QString &aInput) {
    if (!aInput.contains("sigma(")) { return; }

    int sigmaIndex = aInput.indexOf("sigma(") + 5; // takes beginning of string -> +5
    QString parenthesesContent = aInput.mid(sigmaIndex);

    { // remove the stuff after the sigma if there is any
        QList<int> parenthesesArray = getParenthesesArray(aInput.mid(sigmaIndex));
        int firstZero = parenthesesArray.indexOf(0);
        if (firstZero != -1) { // if there is a zero in the parenthesesArray
            parenthesesContent = parenthesesContent.left(firstZero);
        }
    }
    // remove the () at the beginning and at the end
    parenthesesContent = parenthesesContent.left(parenthesesContent.length() - 1).mid(1);

    // separate each part of the sigma
    QString function = parenthesesContent.section(',', 0, -4);
    QString variable = parenthesesContent.section(',', -3, -3);
    int start = parenthesesContent.section(',', -2, -2).toInt();
    int finish = parenthesesContent.section(',', -1, -1).toInt();

    // create the expanded form
    QString expandedForm;
    for (int i = start; i <= finish; ++i) {
        QString temp = function;
        expandedForm.append(temp.replace(QRegularExpression(QString("(?<!\\w)[%1]{1}(?!\\w)").arg(variable)), QString::number(i)));

        if (i < finish) {
            expandedForm.append('+');
        }
    }

    // replace the sigma with the expanded form
    aInput.replace(QString("sigma(" + parenthesesContent + ")"), QString("(" + expandedForm + ")"));

    // re-expand sigma if recursive
    if (aInput.contains("sigma(")) {
        sigmaExpand(aInput);
    }
}

/**
 * Returns false if node doesn't need children
 */
bool Node::needsChildren() {
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

/**
 * Compute the node value by taking the children's value
 */
double Node::computeOperation(double xPlug) const {
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
        case Addition:
            return doubleValueLeft + doubleValueRight;
        case Subtraction:
            return doubleValueLeft - doubleValueRight;
        case Multiplication:
            return doubleValueLeft * doubleValueRight;
        case Division:
            return doubleValueLeft / doubleValueRight;
        case Modulo:
            if (int(doubleValueRight) != 0) {
                return int(doubleValueLeft) % int(doubleValueRight);
            }
            return nan("");
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
        case Floor:
            return floor(doubleValueRight);
        case Ceil:
            return ceil(doubleValueRight);
        default:
            // if function is f(x) = x
            if (!pParent && !pLeftChild && !pRightChild && isVariable) {
                return xPlug;
            }
            // if function is f(x) = 2 for example
            return doubleValue;
    }
}

/**
 * Returns a list that has the same size as param string.
 * This list indicates inside how many parentheses every char
 * of the string is. Take the following example:
 *
 * 5+(x-4)*(e^(x+2))+1 gives
 * 0011111011122222100
 */
QList<int> Node::getParenthesesArray(const QString &string) {
    QList<int> list;
    int insideHowManyParentheses = 0;

    for (const QChar &i : string) {
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

/**
 * Finds all occurrences in param string of param what.
 * Returns a list containing the indexes of the QString what.
 */
QList<int> Node::findAllOccurrences(QString string, const QString &what) {
    QList<int> operatorIndex;
    int cutAwayParts = 0;

    while (string.indexOf(what) != -1) {
        operatorIndex.append(string.indexOf(what) + cutAwayParts);
        cutAwayParts += string.leftRef(string.indexOf(what) + 1).length();

        string = string.mid(string.indexOf(what) + 1);
    }
    return operatorIndex;
}
