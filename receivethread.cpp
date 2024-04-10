#include "receivethread.h"

receiveThread::receiveThread(QObject* parent): QThread(parent)
{

}

receiveThread::~receiveThread()
{
    std::cout << "Thread receiving is closed." <<std::endl;
}
void receiveThread::set(std::string port, uint32_t baudrate)
{
    this->port = port;
    this->baudrate = baudrate;
}


void receiveThread::run()
{
    this->stopFlag = true;

    serial::Serial openedPort(this->port, this->baudrate, serial::Timeout::simpleTimeout(1000));
    if(!openedPort.isOpen())
    {
        std::cout << "serial open failed" << std::endl;
        return;
    }
    else
    {
        std::cout << "Thread start receiving..." << std::endl;
//        std::cout << "MyThread thread id:" << QThread::currentThreadId();
    }

    while(stopFlag)
    {
        int len = openedPort.available();

        if(len > 0)
        {
            std::string data;
            data = openedPort.readline();//这里默认换行是'\n'
            std::cout << "read data:" << data << std::endl;

            this->m_data = QString(QLatin1String(data.c_str()));
            std::cout << m_data.toUtf8().constData() << std::endl;
            std::string data2 = m_data.toUtf8().constData();
            std::cout << data2.size() << std::endl;
            std::cout << static_cast<uint8_t>(data2[0]) << std::endl;
            std::cout << static_cast<int>(static_cast<uint8_t>(data[0])) << std::endl;



            emit receiveData(this->m_data);
        }
    }

}

void receiveThread::stop()
{
    this->stopFlag = false;
    if(isRunning())
    {
        exit();
        wait();
    }
}
