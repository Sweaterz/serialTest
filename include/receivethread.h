#ifndef RECEIVETHREAD_H
#define RECEIVETHREAD_H
#include <iostream>
#include <QThread>
#include <QDebug>
#include <QRunnable>
#include "serial/serial.h"
class receiveThread: public QThread
{
    Q_OBJECT
    std::string port;
    uint32_t baudrate;
public:
    receiveThread(QObject* parent = nullptr);
    ~receiveThread();

    bool stopFlag;
    void set(std::string port, uint32_t baudrate);
    void run();
    void stop();
    void parseData();

signals:
    void receiveData(const QByteArray&);
private:
    QByteArray m_data;

};

#endif // RECEIVETHREAD_H
