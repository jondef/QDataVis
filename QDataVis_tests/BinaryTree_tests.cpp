//
// Created by John on 25.10.2019.
//

#include <QtCore>
#include <iostream>

#include "gtest/gtest.h"
#include "BinaryTree.hpp"

class BinaryTreeFixture : public ::testing::Test {

protected:
	virtual void SetUp() {

	}

	virtual void TearDown() {
	}

	BinaryTree *tree = new BinaryTree();
	QList<QString> functionList = {"x^2", "x^(2/3)"};
	QList<double> resultsList = {25, 2.9240177382128660655067873601379};

};

TEST_F(BinaryTreeFixture, FunctionsCalculationTest) {
	double x = -5;

	for (int i = 0; i < functionList.length(); ++i) {
		tree->setPRoot(const_cast<QString &>(functionList.at(i)));

		EXPECT_EQ(tree->calculateTree(x), resultsList.at(i));
	}

}

TEST(BinaryTree_tests, sample_test) {
	EXPECT_EQ(1, 1);
}

TEST(BinaryTree_tests, ExampleDate) {
	QString str = "x^2";
	BinaryTree tree(str);
	double x = 3;
	double y = tree.calculateTree(x);

	EXPECT_EQ(y, 9);
}
