//
// Created by TAADEJOM on 07.05.2021.
//

#ifndef QDATAVIS_QCPITEMTEXTCUSTOM_HPP
#define QDATAVIS_QCPITEMTEXTCUSTOM_HPP

class QCPItemTextCustom : public QCPItemText {
Q_OBJECT
    using QCPItemText::QCPItemText;
    Q_PROPERTY(QPointF position READ getPosition WRITE setPosition)

public:
    void setPosition(const QPointF &pos) {
        position->setCoords(pos);
    }

    QPointF getPosition() const {
        return position->coords();
    }

    static QVariant QCPTextPosInterpolator(const QPointF &start, const QPointF &end, qreal progress) {
        QVariant done;

        double x;
        if (start.x() <= end.x()) {
            x = start.x() + abs(end.x() - start.x()) * progress;
        } else {
            x = start.x() - abs(end.x() - start.x()) * progress;
        }
        double y;
        if (start.y() <= end.y()) {
            y = start.y() + abs(end.y() - start.y()) * progress;
        } else {
            y = start.y() - abs(end.y() - start.y()) * progress;
        }
        done.setValue(QPointF(x, y));

        return done;
    }
};

#endif //QDATAVIS_QCPITEMTEXTCUSTOM_HPP
