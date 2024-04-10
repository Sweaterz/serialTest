#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initSerial();
    this->move(560, 280);

//    receiveThread *receive_t = new receiveThread;
    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::openSerial);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearInfo);
    connect(ui->updateButton, &QPushButton::clicked, this, &MainWindow::initSerial);
//    connect(ui->receiveDataArea, &QListWidget::mousePressEvent, this, &MainWindow::rightClickClear);


    ui->receiveDataArea->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->receiveDataArea, &QListWidget::customContextMenuRequested, this, &MainWindow::slotCustomMenuRequested);

}

void MainWindow::initSerial()
{
    std::vector<serial::PortInfo> allPortList = serial::list_ports();
    QStringList serialPorts;
    for(auto port: allPortList)
    {
        if(port.hardware_id != "n/a")
        {
//            qDebug() << port.port.c_str() << port.description.c_str() << port.hardware_id.c_str();

            serialPorts.append(port.port.c_str());
        }
    }
    if(serialPorts.size() != 0)
    {
        this->ui->serialBox->clear();
        this->ui->serialBox->addItems(serialPorts);

    }
    else
    {
        this->ui->serialBox->clear();
        this->ui->serialBox->addItem("None");
    }

}

void MainWindow::openSerial()
{
    std::string port = this->ui->serialBox->currentText().toStdString();
    uint32_t baudrate = std::stoi(this->ui->baudrateBox->currentText().toStdString());
    std::string dataBits = this->ui->dataBitsBox->currentText().toStdString();
    std::string stopBits = this->ui->stopBitsBox->currentText().toStdString();
    std::string parity = this->ui->parityBox->currentText().toStdString();

    static receiveThread *receive_t;

    if (this->ui->openButton->text() == "打开")
    {

        std::cout << "open serial " << std::endl;
        try
        {
            serial::Serial openedPort(port, baudrate, serial::Timeout::simpleTimeout(1000));
        }
        catch(serial::IOException e)
        {
            //        std::cout << e.what() << std::endl;
            QMessageBox::warning(NULL, "Title", e.what());
            return;
        }
        this->ui->openButton->setText("关闭");

        receive_t = new receiveThread();
        connect(receive_t, &receiveThread::receiveData, this, &MainWindow::showData);

        receive_t->set(port, baudrate);
        receive_t->start();


    }
    else if (this->ui->openButton->text() == "关闭")
    {
        receive_t->stop();
        this->ui->openButton->setText("打开");
        disconnect(receive_t, &receiveThread::receiveData, this, &MainWindow::showData);

        delete receive_t;
    }


}


void MainWindow::showData(const QString& data)
{
//    std::cout << data.toStdString() << std::endl;
    int length = data.size();
    if(this->ui->hexOutput->isChecked())
    {
        std::cout << "十六进制输出模式" << std::endl;
        bool ok;
        std::vector<uint8_t> hexData;
        std::vector<std::string> hexStrData;

        for(int i = 0; i < data.size(); i++)
        {
            std::cout << data[i].unicode() << std::endl;
            hexData.push_back(data[i].unicode());
        }
        std::cout << data.toStdString() << std::endl;

    }
    else
    {
        QString dataRemoveN = data.left(length - 1);
        std::cout << "原始文本输出模式" << std::endl;
        this->ui->receiveDataArea->addItem(dataRemoveN);
    }

    this->ui->receiveDataArea->setCurrentRow(this->ui->receiveDataArea->count() - 1);
}

void MainWindow::clearInfo()
{
    qDebug() << "clear info";
    this->ui->receiveDataArea->clear();
}

//实现右键菜单清除功能
void MainWindow::slotCustomMenuRequested(QPoint pos)
{

    QMenu *menu = new QMenu(this);
    QAction* clearInfo = new QAction("清除信息", this);
    connect(clearInfo, &QAction::triggered, this, &MainWindow::clearInfo);
    menu->addAction(clearInfo);
//    menu->addAction(new QAction("Action 2", this));
//    menu->addAction(new QAction("Action 3", this));
    menu->popup(ui->receiveDataArea->mapToGlobal(pos));
}



MainWindow::~MainWindow()
{
    delete ui;
}

