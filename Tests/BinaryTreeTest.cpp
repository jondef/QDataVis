//
// Created by jon on 18.07.20.
//

#include <QTest>
#include "BinaryTree.hpp"

#define myDebug() qDebug() << Qt::fixed << qSetRealNumberPrecision(16)

inline double roundNumber(double value) {
	return round(value * 1000.0) / 1000.0;
}


class BinaryTreeTest : public QObject {
Q_OBJECT
private slots:

	[[maybe_unused]] static void t1() {
		BinaryTree *tree = new BinaryTree("-sqrt(1-x^2)");
		QVERIFY(tree->calculateTree(1.0) == 0);
		QVERIFY(tree->calculateTree(-1.0) == 0);
		QVERIFY(tree->calculateTree(0) == -1);
	}

	[[maybe_unused]] static void t2() {
		BinaryTree *tree = new BinaryTree("sqrt(-x^2-4*x+46)-4");
		QVERIFY(tree->calculateTree(-2) == 3.0710678118654755);
		QVERIFY(roundNumber(tree->calculateTree(-7.830951894845301)) == 0);
		QVERIFY(roundNumber(tree->calculateTree(3.8309518948453007)) == 0);
	}

	[[maybe_unused]] static void t3() {
		BinaryTree *tree = new BinaryTree("-x-1");
		QVERIFY(tree->calculateTree(0) == -1);
		QVERIFY(tree->calculateTree(-1000) == 999);
	}

	[[maybe_unused]] static void t4() {
		BinaryTree *tree = new BinaryTree("3/((5x+3)(-6x+4))");
		QVERIFY(tree->calculateTree(0) == 0.25);
	}
};

QTEST_MAIN(BinaryTreeTest)

#include "BinaryTreeTest.moc"
