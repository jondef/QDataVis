//
// Created by jon on 29.06.20.
//

#ifndef QDATAVIS_GRAPH_H
#define QDATAVIS_GRAPH_H

#include <qcp.h>

#include <utility>
#include "BinaryTree.hpp"

struct DataSet : public QThread {
Q_OBJECT

public:
    DataSet() {
        listWidgetItem = nullptr;
        graph = nullptr;
        binaryTree = nullptr;
    }

    ~DataSet() {
        delete listWidgetItem;
        delete graph;
        delete binaryTree;
    }

    enum Type {
        FunctionGraph,
        PointGraph
    };

    Type getDataSetType() const {
        return (binaryTree && !overrideGlobalPointDensity) ? FunctionGraph : PointGraph;
    }

    bool dataSetIsFunction() const {
        return getDataSetType() == FunctionGraph;
    }


    void changeColor(QColor aColor) {
        this->color = aColor;
        // * update the color on the list widget item
        QPixmap pixmap = QPixmap(16, 16);
        pixmap.fill(this->color);
        this->listWidgetItem->setIcon(QIcon(pixmap));
        // * update the graph
        QPen graphPen;
        graphPen.setColor(this->color);
        graphPen.setWidthF(this->graphWidth); // between 1 and 2 acceptable (float/int)
        this->graph->setPen(graphPen);
//		this->graph->setBrush(QBrush(QColor(0, 0, 255, 20))); // set background
    }

    void configureListWidgetItem() {
        QVariant variant;
        variant.setValue(this);
        listWidgetItem->setData(Qt::UserRole, variant);
        listWidgetItem->setText(this->displayName);
    }

    void run() override {
        emit resultReady(binaryTree->calculateTree(minimumDomain, maximumDomain, pointDensity));
    }

    inline void recalculatePoints() const {
        graph->setData(binaryTree->calculateTree(minimumDomain, maximumDomain, pointDensity));
    }

    static QVector<double> generateXArray(double lowerLim, double upperLim, unsigned int length) {
        QVector<double> finalArray(length);

        double difference = upperLim - lowerLim;
        double increment = difference / (length - 1);

        for (unsigned int i = 0; i < length; ++i) {
            finalArray[i] = lowerLim + increment * i;
        }
        return finalArray;
    }

    /**
     * This method does a linear regression on the points of the dataset.
     * @return QPair containing 'a' and 'b' numbers of a linear function f(x)=ax+b
     */
    [[nodiscard]] QPair<double, double> linearRegression() const {
        QSharedPointer<QCPGraphDataContainer> points = graph->data();
        int numPoints = points->size();

        QVector<QPair<double, double>> matrix_A;
        for (int i = 0; i < numPoints; ++i) {
            matrix_A.append(QPair<double, double>(points->at(i)->value, 1));
        }

        QPair<QVector<double>, QVector<double>> matrix_AexpT;
        for (int i = 0; i < numPoints; ++i) {
            matrix_AexpT.first.append(points->at(i)->key); // append x coords
            matrix_AexpT.second.append(1); // append the 1s
        }
        // a1 = x1 * x1 + x2 * x2 + x3 * x3...
        double a1 = 0;
        for (const double &x : matrix_AexpT.first) {
            a1 += x * x;
        }
        // b1 = x1 * 1 + x2 * 1 + x3 * 1...
        double b1 = 0;
        for (const double &x : matrix_AexpT.first) {
            b1 += x;
        }
        // c1 = 1 * x1 + 1 * x2 + 1 * x3...
        double c1 = 0;
        for (const double &x : matrix_AexpT.first) {
            c1 += x;
        }
        // d1 = 1 * 1 + 1 * 1 + 1 * 1...
        double d1 = 0;
        for (const double &x : matrix_AexpT.second) {
            d1 += x;
        }

        QVector<double> solvector; // y coordinates of points
        for (const QCPGraphData &i : *points) {
            solvector.append(i.value);
        }
        // g = y1 * x1 + y2 * x2 + y3 * x3...
        double g = 0;
        for (int x = 0; x < numPoints; ++x) {
            g += solvector.at(x) * matrix_AexpT.first.at(x);
        }
        // h = y1 * 1 + y2 * 1 + y3 * 1...
        double h = 0;
        for (int x = 0; x < numPoints; ++x) {
            h += solvector.at(x) * matrix_AexpT.second.at(x);
        }

        double a = (g * d1 - b1 * h) / (d1 * a1 - b1 * c1);
        double b = h / d1 - (c1 / d1 * a);
        return QPair<double, double>(a, b);
    }

    /**
     * This method does a regression on the data set.
     * @param degree: degree of the regression (ex: 1 for linear or 2 for quadratic)
     * @return QList<double> containing the coefficients from biggest to smallest.
     */
    [[nodiscard]] QVector<double> regression(int degree) const;

    static QString getFunctionString(const QVector<double>& coeffs);

    QString displayName;
    QString functionString;
    QListWidgetItem *listWidgetItem;
    QCPGraph *graph;
    BinaryTree *binaryTree;
    QColor color;
    double graphWidth = 2.5;

    bool overrideGlobalPointDensity = false;
    unsigned int pointDensity = 1000;
    double minimumDomain = -10;
    double maximumDomain = 10;
signals:
    void resultReady(QSharedPointer<QCPGraphDataContainer> data);

};
Q_DECLARE_METATYPE(DataSet *)
QDataStream &operator<<(QDataStream &out, const DataSet &dataSet);
QDataStream &operator>>(QDataStream &in, DataSet &dataSet);

#endif //QDATAVIS_GRAPH_H
