//
// Created by TAADEJOM on 15.05.2021.
//

#include "DataSet.hpp"
#include <iostream>
#include <Eigen/Dense>
using namespace Eigen;

QList<double> DataSet::regression(unsigned int degree) const {
    QSharedPointer<QCPGraphDataContainer> points = graph->data();
    int numPoints = points->size();

    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    std::cout << "Here is the matrix m:\n" << m << std::endl;
    VectorXd v(2);
    v(0) = 4;
    v(1) = v(0) - 1;
    std::cout << "Here is the vector v:\n" << v << std::endl;


    return QList<double>(1.0, 1.0);
}