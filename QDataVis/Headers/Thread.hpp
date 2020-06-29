//
// Created by John on 24.08.2019.
//

#ifndef GUI_APP_THREAD_H
#define GUI_APP_THREAD_H

#include <QThread>


class Thread : public QThread {
Q_OBJECT

public:
    explicit Thread(QObject *parent = 0);

    void run();


private:

};


#endif //GUI_APP_THREAD_H