//
// Created by jon on 29.06.20.
//

#ifndef QDATAVIS_GRAPH_H
#define QDATAVIS_GRAPH_H

#include <qcustomplot.h>
#include "BinaryTree.hpp"

// todo: refactor to DataSet
struct Graph {
	Graph() = default;

	Graph(QCPGraph *aGraph, BinaryTree *aBinaryTree) {
		QCPGraph *graph = aGraph;
		BinaryTree *binaryTree = aBinaryTree;
	}

	QString name;
	QListWidgetItem *listWidgetItem;
	QCPGraph *graph;
	BinaryTree *binaryTree;

	bool followGlobalPointDensity = true;
};


#endif //QDATAVIS_GRAPH_H
