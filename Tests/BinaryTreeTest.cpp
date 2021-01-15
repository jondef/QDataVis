//
// Created by jon on 18.07.20.
//

#include <QTest>
#include "BinaryTree.hpp"

#define myDebug() qDebug() << Qt::fixed << qSetRealNumberPrecision(16)

inline double roundNumber(double value) {
    return round(value * 1000.0) / 1000.0;
}


/*
 * todo: test the following functions:
 * sigma((x^(2*n+1))/(x*n^2),n,0,20)
 * sigma(cos(x*n),n,0,20)
 * sigma((1/n)*cos(x*n),n,0,20)
 */

class BinaryTreeTest : public QObject {
Q_OBJECT
private slots:

    [[maybe_unused]] static void t1() {
        BinaryTree tree = BinaryTree("-sqrt(1-x^2)");
        QVERIFY(tree.calculateTree(1.0) == 0);
        QVERIFY(tree.calculateTree(-1.0) == 0);
        QVERIFY(tree.calculateTree(0) == -1);
    }

    [[maybe_unused]] static void t2() {
        BinaryTree tree = BinaryTree("sqrt(-x^2-4x+46)-4");
        QVERIFY(tree.calculateTree(-2) == 3.0710678118654755);
        QVERIFY(roundNumber(tree.calculateTree(-7.830951894845301)) == 0);
        QVERIFY(roundNumber(tree.calculateTree(3.8309518948453007)) == 0);
    }

    [[maybe_unused]] static void t3() {
        BinaryTree tree = BinaryTree("-x-1");
        QVERIFY(tree.calculateTree(0) == -1);
        QVERIFY(tree.calculateTree(-1000) == 999);
    }

    [[maybe_unused]] static void t4() {
        BinaryTree tree = BinaryTree("3/((5x+3)(-6x+4))");
        QVERIFY(tree.calculateTree(0) == 0.25);
    }

    [[maybe_unused]] static void t5() {
        BinaryTree func1 = BinaryTree("3/4*sin(3/2*pi*x-pi/3)+1/2*cos(5/2*pi*x+pi/4)");
        BinaryTree func2 = BinaryTree("-3/8*sqrt(3)*cos(3/2*pi*x)+1/4*sqrt(2)*cos(5/2*pi*x)+3/8*sin(3/2*pi*x)-1/4*sqrt(2)*sin(5/2*pi*x)");
        for (int i = -10; i < 10; ++i) {
            QVERIFY(roundNumber(func1.calculateTree(i)) == roundNumber(func2.calculateTree(i)));
        }
    }

    [[maybe_unused]] static void t6() {
        BinaryTree func1("3/4*sqrt(2x)");
        BinaryTree func2("3/4sqrt(2x)");
        BinaryTree func3("(3/4)sqrt(2x)");
        QVERIFY(roundNumber(func1.calculateTree(0)) == 0);
        QVERIFY(roundNumber(func1.calculateTree(1)) == 1.0609999999999999);
        QVERIFY(roundNumber(func1.calculateTree(100)) == 10.6069999999999993);
        for (int i = 0; i < 10; ++i) {
            QVERIFY(roundNumber(func1.calculateTree(i)) == roundNumber(func2.calculateTree(i)));
            QVERIFY(roundNumber(func2.calculateTree(i)) == roundNumber(func3.calculateTree(i)));
        }
    }

    [[maybe_unused]] static void t7() {
        BinaryTree func1("sigma((-1)^n, n, 1, 10)");
        BinaryTree func2("sigma((-1)^n, n, 1, 11)");
        for (int i = -10; i < 10; ++i) {
            QVERIFY(roundNumber(func1.calculateTree(i)) == 0);
            QVERIFY(roundNumber(func2.calculateTree(i)) == -1);
        }
    }

    [[maybe_unused]] static void t8() {
        // this function from 0 to 1000 gives nan because (x^647 | x = 3 is equal to inf) and inf * 0 is nan
        BinaryTree func1("sigma(1/k! * x^k, k, 0, 100)");
        BinaryTree func2("e^x");
        for (int i = 0; i < 10; ++i) {
            QVERIFY(roundNumber(func1.calculateTree(i)) == roundNumber(func2.calculateTree(i)));
        }
    }
};

QTEST_MAIN(BinaryTreeTest)

#include "BinaryTreeTest.moc"
