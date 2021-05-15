//
// Created by TAADEJOM on 15.05.2021.
//

#include "DataSet.hpp"
#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;

MatrixXd vConcat(MatrixXd A, MatrixXd B);

QVector<double> DataSet::regression(int degree) const {
    QSharedPointer<QCPGraphDataContainer> data = graph->data();

    // xPoints = data(:,1);  %x -> A
    // yPoints = data(:,2); %y -> b
    MatrixXd xPoints(data->size(), 1); // n rows, 1 column
    MatrixXd yPoints(data->size(), 1); // n rows, 1 column
    for (int i = 0; i < data->size(); ++i) {
        xPoints(i, 0) = data->at(i)->key;
        yPoints(i, 0) = data->at(i)->value;
    }

    int m = data->size(); // rows
    int n = degree; // columns

    // x0 = ones(m,1).*80.3e3;
    MatrixXd x1 = xPoints;
    MatrixXd x0 = Eigen::MatrixXd::Ones(m, 1) * 80300;

    MatrixXd z1 = (x1 - x0).array() / 50;

    // Set up vector A
    MatrixXd A(m, n);
    for (int i = 1; i <= n; ++i) {
        if (i == n) {
            // A(1:m, degree) = ones(m,1);
            A.col(i - 1) = Eigen::MatrixXd::Ones(m, 1);
        } else {
            // A(1:m, degree) = xPoints.^(n - degree);
            A.col(i - 1) = xPoints.array().pow(n - i);
        }
    }

    MatrixXd Im = Eigen::MatrixXd::Identity(m, m); // m*m identity matrix

    // QR-Zerlegung von A
    MatrixXd R = A;
    MatrixXd Q = Im;
    for (int k = 1; k <= n; ++k) {
//        std::cout << "LOOP ITERATION: " << k << std::endl;
        MatrixXd x = R.col(k - 1).bottomRows(R.rows() - k + 1); // we wanna show R.rows - k rows from the bottom // fixme: ErrorRowIndexOutOfBounds with 5 data points and degree 10
        double lambda;

//        std::cout << x << std::endl;
//        std::cout << x.coeff(k,k) << std::endl;
        if (x.coeff(k, k) < 0) {
            lambda = x.norm();
        } else {
            lambda = -x.norm();
        }
//        MatrixXd e = Im.block(k, k, m, n);
        MatrixXd e = Im.col(k - 1).bottomRows(R.rows() - k + 1);
        MatrixXd v = (x - lambda * e) / (x - lambda * e).norm(); // use normalize()
        MatrixXd z = Eigen::MatrixXd::Zero(k - 1, 1);

        // H = eye(m)-2.*[z;v]*[z;v]';
        MatrixXd H = Eigen::MatrixXd::Identity(m, m) - (2 * (vConcat(z, v) * vConcat(z, v).transpose()));

        Q = Q * H;
//        std::cout << e << std::endl;
        R = H * R;
    }
//    std::cout << R << std::endl << "FUUUUUCK" << std::endl << Q;
    // Rückwärtseinsetzen
    MatrixXd y = Q.transpose() * yPoints;

    MatrixXd x = Eigen::MatrixXd::Zero(1, n);
    // x(n) = y(n)/R(n,n);
//    std::cout << "FUCK" << std::endl;
    x.coeffRef(0, n - 1) = y.coeff(n  - 1, 0) / R.coeff(n - 1, n - 1);
//    std::cout << "FUCK" << std::endl;
    for (int i = (n - 1); i >= 1; --i) {
        double s = 0;
        for (int j = i + 1; j <= n; ++j) {
            // s = s + x(j)*R(i,j);
            s += x.coeff(0, j - 1) * R.coeff(i - 1, j - 1);
        }
        // x(i) = (y(i) - s)/R(i,i);
        x.coeffRef(i - 1, 0) = (y.coeff(i - 1, 0) - s) / R.coeff(i - 1, i - 1);
    }

    MatrixXd fx = A * x.transpose();

    return QVector<double>(fx.data(), fx.data() + fx.rows() * fx.cols());
}


MatrixXd vConcat(MatrixXd A, MatrixXd B) {
    MatrixXd D(A.rows()+B.rows(), A.cols()); // <-- D(A.rows() + B.rows(), ...)
    D << A, B;
    return D;
}