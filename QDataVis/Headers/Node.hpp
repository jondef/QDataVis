//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_NODE_H
#define GUI_APP_NODE_H

#include <QtCore>

// todo: add more functions, operators: http://www.partow.net/programming/exprtk/
enum Operator {
    Addition, Subtraction, Multiplication, Division, Modulo, Exponent, Factorial,
    Arcsinh, Arccosh, Arctanh, Arcsin, Arccos, Arctan, Sinh, Cosh, Tanh, Sin, Cos, Tan, Log10, Log, Ln, Abs, Sqrt, Cbrt, Ceil, Floor
};


const QVector<QPair<QString, Operator>> operatorsPriority = {
        {"+", Addition},
        {"-", Subtraction},
        {"*", Multiplication},
        {"/", Division},
        {"%", Modulo},
        {"^", Exponent},
        {"!", Factorial}
};
// ! all "arc" trigo functions need to be before the normal ones
const QVector<QPair<QString, Operator>> specialOperatorsPriority = {
        {"arcsinh", Arcsinh},
        {"arccosh", Arccosh},
        {"arctanh", Arctanh},
        {"arcsin",  Arcsin},
        {"arccos",  Arccos},
        {"arctan",  Arctan},
        {"sinh",    Sinh},
        {"cosh",    Cosh},
        {"tanh",    Tanh},
        {"sin",     Sin},
        {"cos",     Cos},
        {"tan",     Tan},
        {"log10",   Log10},
        {"log",     Log},
        {"ln",      Ln},
        {"abs",     Abs},
        {"sqrt",    Sqrt},
        {"cbrt",    Cbrt},
        {"floor",   Floor},
        {"ceil",    Ceil},
};


class Node {

public:
    explicit Node(QString &input, Node *aParent = nullptr);

    ~Node();

    [[nodiscard]] double computeOperation(double xPlug) const;

    static QList<int> getParenthesesArray(const QString &string);

private:
    static QList<int> findAllOccurrences(QString string, const QString &what);

    bool createChildren(QString string);

    bool needsChildren();

    void sigmaExpand(QString &aInput);

public:
    Node *pParent = nullptr; // ! do not delete this pointer in the destructor
    Node *pLeftChild = nullptr;
    Node *pRightChild = nullptr;

    QString strValue;
    double doubleValue;
    Operator mathOperation = static_cast<Operator>(-1);
    double logBase;
    bool hasChildren = false;
    bool isVariable = false;
};


#endif //GUI_APP_NODE_H
