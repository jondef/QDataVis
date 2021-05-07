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

    QRect getRect() const {
        return clipRect();
    }
};

#endif //QDATAVIS_QCPITEMTEXTCUSTOM_HPP
